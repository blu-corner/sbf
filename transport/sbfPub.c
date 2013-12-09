#include "sbfTport.h"
#include "sbfTportPrivate.h"

static void
sbfPubMakeHeader (sbfPub pub)
{
    sbfTportHeader* hdr;
    const char*     topic;
    size_t          topicSize;

    topic = sbfTopic_getTopic (pub->mTopic);
    topicSize = strlen (topic) + 1;

    pub->mHeaderSize = (((sizeof *hdr) + topicSize) & ~7) + 8;
    pub->mHeader = xcalloc (1, pub->mHeaderSize);

    pub->mHeader->mTopicSize = pub->mHeaderSize - sizeof *hdr;
    memcpy (pub->mHeader + 1, topic, topicSize);
}

static inline void
sbfPubFree (sbfPub pub)
{
    free (pub->mHeader);

    sbfTopic_destroy (pub->mTopic);
    free (pub);
}

static void
sbfPubAddEventCb (int fd, short events, void* closure)
{
    sbfPub         pub0 = closure;
    sbfTopic       topic = pub0->mTopic;
    sbfTportStream tstream = pub0->mTportStream;
    sbfTportTopic  ttopic;

    sbfLog_debug ("adding %p", pub0);

    ttopic = sbfTport_findTopic (tstream, sbfTopic_getTopic (topic));
    if (ttopic == NULL)
        ttopic = sbfTport_addTopic (tstream, sbfTopic_getTopic (topic));
    sbfLog_debug ("using topic %p (stream %p) for %p", ttopic, tstream, pub0);

    TAILQ_INSERT_TAIL (&ttopic->mPubs, pub0, mEntry);
    pub0->mTportTopic = ttopic;

    if (tstream->mReady)
        sbfPub_ready (pub0);
}

static void
sbfPubAddStreamCompleteCb (sbfHandlerHandle handle,
                           sbfError error,
                           void* closure)
{
    sbfPub         pub0 = closure;
    sbfTport       tport = pub0->mTport;
    sbfTportStream tstream;
    sbfTportTopic  ttopic;
    sbfPub         pub;

    if (pub0->mTportStream == NULL || pub0->mTportStream->mStream == NULL)
    {
        /*
         * Take and release lock to synchronize AddStream return being copied
         * into the member.
         */
        pthread_mutex_lock (&tport->mStreamsLock);
        pthread_mutex_unlock (&tport->mStreamsLock);
    }
    tstream = pub0->mTportStream;

    if (error != 0)
    {
        /*
         * If stream add fails, just return - publishers on this stream will
         * never become ready.
         */
        sbfLog_err ("failed to add stream %p: %s", tstream, strerror (error));
        return;
    }

    sbfLog_debug ("stream %p is ready (for %p): thread %u",
                  tstream,
                  pub0,
                  sbfMw_getThreadIndex (tstream->mThread));

    tstream->mReady = 1;

    RB_FOREACH (ttopic, sbfTportTopicTreeImpl, &tstream->mTopics)
    {
        TAILQ_FOREACH (pub, &ttopic->mPubs, mEntry)
            sbfPub_ready (pub);
    }

    sbfPubAddEventCb (-1, 0, pub0);
}

static void
sbfPubRemoveEventCb (int fd, short events, void* closure)
{
    sbfPub         pub0 = closure;
    sbfTport       tport = pub0->mTport;
    sbfTportTopic  ttopic = pub0->mTportTopic;
    sbfTportStream tstream = pub0->mTportStream;

    sbfLog_debug ("removing %p", pub0);

    TAILQ_REMOVE (&ttopic->mPubs, pub0, mEntry);
    if (TAILQ_EMPTY (&ttopic->mPubs) && sbfTport_removeTopic (tstream, ttopic))
    {
        pthread_mutex_lock (&tport->mStreamsLock);
        sbfTport_removeStream (tport, tstream);
        pthread_mutex_unlock (&tport->mStreamsLock);
    }

    if (sbfRefCount_decrement (&pub0->mRefCount))
        sbfPubFree (pub0);
}

static void
sbfPubSetStream (sbfPub pub)
{
    sbfTport       tport = pub->mTport;
    sbfTportStream tstream;

    pthread_mutex_lock (&tport->mStreamsLock);
    tstream = tport->mHandlerTable->mFindStream (tport->mHandler, pub->mTopic);
    if (tstream != NULL)
    {
        sbfMw_enqueue (tstream->mThread,
                       &pub->mEventAdd,
                       sbfPubAddEventCb,
                       pub);
    }
    else
    {
        pub->mTportStream = sbfTport_addStream (tport,
                                                pub->mTopic,
                                                sbfPubAddStreamCompleteCb,
                                                pub);
    }
    pthread_mutex_unlock (&tport->mStreamsLock);
}

static void
sbfPubReadyQueueCb (sbfQueueItem item, void* closure)
{
    sbfPub pub = closure;

    if (!pub->mDestroyed)
        pub->mReadyCb (pub, pub->mClosure);

    if (sbfRefCount_decrement (&pub->mRefCount))
        sbfPubFree (pub);
}

void
sbfPub_ready (sbfPub pub)
{
    sbfLog_debug ("%p is ready", pub);
    pub->mReady = 1;

    if (!pub->mDestroyed && pub->mReadyCb != NULL)
    {
        sbfRefCount_increment (&pub->mRefCount);
        sbfQueue_enqueue (pub->mQueue, sbfPubReadyQueueCb, pub);
    }
}

sbfPub
sbfPub_create (sbfTport tport,
               sbfQueue queue,
               const char* topic0,
               sbfPubReadyCb readyCb,
               void* closure)
{
    sbfPub   pub;
    sbfTopic topic;

    topic = sbfTopic_create (SBF_TOPIC_PUB, topic0);
    if (topic == NULL)
        return NULL;

    pub = xcalloc (1, sizeof *pub);
    pub->mTopic = topic;

    pub->mDestroyed = 0;
    sbfRefCount_init (&pub->mRefCount, 1);

    pub->mReady = 0;

    pub->mTport = tport;
    pub->mTportStream = NULL;
    pub->mTportTopic = NULL;

    sbfLog_debug ("creating %p: topic %s",
                  pub,
                  sbfTopic_getTopic (pub->mTopic));

    pub->mQueue = queue;
    pub->mReadyCb = readyCb;
    pub->mClosure = closure;

    sbfPubMakeHeader (pub);
    sbfPubSetStream (pub);

    return pub;
}

void
sbfPub_destroy (sbfPub pub)
{
    if (pub->mDestroyed)
        return;
    pub->mDestroyed = 1;

    sbfMw_enqueue (pub->mTportStream->mThread,
                   &pub->mEventRemove,
                   sbfPubRemoveEventCb,
                   pub);
}

sbfBuffer
sbfPub_getBuffer (sbfPub pub, size_t size)
{
    sbfTport        tport = pub->mTport;
    sbfTportStream  tstream = pub->mTportStream;
    sbfBuffer       buffer;
    size_t          wanted;
    sbfTportHeader* hdr;

    if (pub->mDestroyed || !pub->mReady)
        return sbfBuffer_new (NULL, size);

    wanted = pub->mHeaderSize + size;
    buffer = tport->mHandlerTable->mGetBuffer (tstream->mStream, wanted);

    hdr = sbfBuffer_getData (buffer);
    memcpy (hdr, pub->mHeader, pub->mHeaderSize);
    sbfBuffer_setData (buffer, (char*)hdr + pub->mHeaderSize);
    sbfBuffer_setSize (buffer, size);

    buffer->mOwner = pub;
    return buffer;
}

void
sbfPub_sendBuffer (sbfPub pub, sbfBuffer buffer)
{
    sbfTportStream  tstream = pub->mTportStream;
    sbfTportHeader* hdr;
    size_t          size;

    if (pub->mDestroyed || !pub->mReady)
        return;

    size = sbfBuffer_getSize (buffer);
    if (size > SBF_MESSAGE_SIZE_LIMIT)
    {
        sbfBuffer_destroy (buffer);
        return;
    }

    if (buffer->mOwner != pub || sbfRefCount_get (&buffer->mRefCount) > 1)
    {
        sbfPub_send (pub, sbfBuffer_getData (buffer), size);
        sbfBuffer_destroy (buffer);
        return;
    }

    hdr = (void*)((char*)sbfBuffer_getData (buffer) - pub->mHeaderSize);
    hdr->mSize = sbfBuffer_getSize (buffer);

    sbfBuffer_setData (buffer, hdr);
    sbfBuffer_setSize (buffer, hdr->mSize + pub->mHeaderSize);

    pthread_mutex_lock (&tstream->mSendLock);
    pub->mTport->mHandlerTable->mSendBuffer (tstream->mStream, buffer);
    pthread_mutex_unlock (&tstream->mSendLock);

    sbfBuffer_destroy (buffer);
}

void
sbfPub_send (sbfPub pub, void* data, size_t size)
{
    sbfBuffer buffer;

    buffer = sbfPub_getBuffer (pub, size);
    memcpy (sbfBuffer_getData (buffer), data, size);

    sbfPub_sendBuffer (pub, buffer);

    sbfBuffer_destroy (buffer);
}

sbfTopic
sbfPub_getTopic (sbfPub pub)
{
    return pub->mTopic;
}
