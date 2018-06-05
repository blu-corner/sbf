#include "sbfTport.h"
#include "sbfTportPrivate.h"

static size_t
sbfBatchNextBuffer (sbfBatchItem item, sbfPub pub)
{
    sbfTportStream   tstream = item->mTportStream;
    sbfHandlerTable* table = tstream->mTport->mHandlerTable;
    size_t           left;

    if (item->mCurrent == NULL)
        left = 0;
    else
        left = table->mPacketSize - sbfBuffer_getSize (item->mCurrent);
    if (left <= pub->mHeaderSize)
    {
        SBF_ASSERT (item->mLast != NULL || item->mCurrent == NULL);
        if (item->mLast != NULL)
        {
            item->mLast->mFlags |= SBF_MESSAGE_FLAG_LAST_IN_PACKET;
            item->mLast = NULL;
        }

        item->mCurrent = sbfTport_getBuffer (tstream, table->mPacketSize);
        sbfDeque_pushBack (item->mBuffers, item->mCurrent);

        sbfBuffer_setSize (item->mCurrent, 0);
    }
    return table->mPacketSize - sbfBuffer_getSize (item->mCurrent);
}

static sbfTportHeader
sbfBatchUpdateBuffer (sbfBatchItem item, sbfPub pub, void* data, size_t size)
{
    sbfTportStream   tstream = item->mTportStream;
    sbfHandlerTable* table = tstream->mTport->mHandlerTable;
    sbfTportHeader   hdr;
    size_t           used = sbfBuffer_getSize (item->mCurrent);

    SBF_ASSERT (used + pub->mHeaderSize + size <= table->mPacketSize);

    hdr = (sbfTportHeader)((char*)sbfBuffer_getData (item->mCurrent) + used);
    memcpy (hdr, pub->mHeader, pub->mHeaderSize);
    hdr->mSize = size;
    hdr->mFlags = 0;

    memcpy ((char*)hdr + pub->mHeaderSize, data, size);
    sbfBuffer_setSize (item->mCurrent, used + pub->mHeaderSize + size);

    item->mLast = hdr;
    return hdr;
}

static void
sbfBatchRemoveItem (sbfBatch batch, sbfBatchItem item)
{
    sbfBuffer buffer;

    while ((buffer = sbfDeque_popFront (item->mBuffers)) != NULL)
        sbfBuffer_destroy (buffer);
    sbfDeque_destroy (item->mBuffers);

    RB_REMOVE (sbfBatchItemTreeImpl, &batch->mTree, item);
    sbfPool_put (item);
}

sbfBatch
sbfBatch_create (sbfTport tport)
{
    sbfBatch batch;

    batch = xcalloc (1, sizeof *batch);
    batch->mTport = tport;
    sbfMutex_init (&batch->mMutex, 0);

    batch->mPool = sbfPool_create (sizeof (struct sbfBatchItemImpl));
    RB_INIT (&batch->mTree);

    return batch;
}

void
sbfBatch_destroy (sbfBatch batch)
{
    sbfBatchItem item;
    sbfBatchItem item1;

    RB_FOREACH_SAFE (item, sbfBatchItemTreeImpl, &batch->mTree, item1)
        sbfBatchRemoveItem (batch, item);
    sbfPool_destroy (batch->mPool);

    sbfMutex_destroy (&batch->mMutex);
    free (batch);
}

void
sbfBatch_add (sbfBatch batch, sbfPub pub, void* data, size_t size)
{
    sbfTportStream          tstream = pub->mTportStream;
    sbfBatchItem            item;
    struct sbfBatchItemImpl impl;
    sbfTportHeader          hdr;
    size_t                  wanted;
    size_t                  left;
    uint8_t                 number;

    if (!sbfTport_checkMessageSize (pub, size))
        return;
    if (SBF_UNLIKELY (size == 0 || pub->mDestroyed || !pub->mReady))
        return;

    sbfMutex_lock (&batch->mMutex);

    impl.mTportStream = tstream;
    if ((item = RB_FIND (sbfBatchItemTreeImpl, &batch->mTree, &impl)) == NULL)
    {
        item = sbfPool_getZero (batch->mPool);
        item->mTportStream = tstream;
        RB_INSERT (sbfBatchItemTreeImpl, &batch->mTree, item);

        item->mCurrent = NULL;
        item->mBuffers = sbfDeque_create ();
    }
    item->mMessages++;

    left = sbfBatchNextBuffer (item, pub);
    if (left > pub->mHeaderSize + size)
        hdr = sbfBatchUpdateBuffer (item, pub, data, size);
    else
    {
        number = 1;
        while (size != 0)
        {
            wanted = SBF_MIN (size, left - pub->mHeaderSize);

            hdr = sbfBatchUpdateBuffer (item, pub, data, wanted);
            hdr->mFragNum = number++;

            data = (char*)data + wanted;
            size -= wanted;

            if (size != 0)
                left = sbfBatchNextBuffer (item, pub);
        }
        if (hdr->mFragNum != 0)
            hdr->mFlags |= SBF_MESSAGE_FLAG_LAST_FRAGMENT;
    }

    sbfMutex_unlock (&batch->mMutex);
}

void
sbfBatch_addBuffer (sbfBatch batch, sbfPub pub, sbfBuffer buffer)
{
    sbfBatch_add (batch,
                  pub,
                  sbfBuffer_getData (buffer),
                  sbfBuffer_getSize (buffer));
}

void
sbfBatch_send (sbfBatch batch)
{
    sbfBatchItem item;
    sbfBatchItem item1;
    sbfBuffer    buffer;

    sbfMutex_lock (&batch->mMutex);

    RB_FOREACH_SAFE (item, sbfBatchItemTreeImpl, &batch->mTree, item1)
    {
        if (item->mLast != NULL)
            item->mLast->mFlags |= SBF_MESSAGE_FLAG_LAST_IN_PACKET;

        item->mMessages = 0;

        while ((buffer = sbfDeque_popFront (item->mBuffers)) != NULL)
        {
            sbfMutex_lock (&item->mTportStream->mSendLock);
            sbfTport_sendBufferLocked (item->mTportStream, buffer);
            sbfMutex_unlock (&item->mTportStream->mSendLock);

            sbfBuffer_destroy (buffer);
        }
        item->mCurrent = NULL;
        item->mLast = NULL;
    }

    sbfMutex_unlock (&batch->mMutex);
}
