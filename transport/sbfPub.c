#include "sbfTport.h"
#include "sbfTportPrivate.h"

static void
sbfPubMakeHeader (sbfPub pub)
{
    sbfTportHeader hdr;
    const char*    topic;
    size_t         topicSize;

    topic = sbfTopic_getTopic (pub->mTopic);
    topicSize = strlen (topic) + 1;

    pub->mHeaderSize = (((sizeof *hdr) + topicSize) & ~7) + 8;
    pub->mHeader = xcalloc (1, pub->mHeaderSize);

    pub->mHeader->mTopicSize = pub->mHeaderSize - sizeof *hdr;
    memcpy (pub->mHeader + 1, topic, topicSize);
}

static SBF_INLINE sbfTportStream
sbfPubEnsureStream (sbfPub pub)
{
    sbfTport tport = pub->mTport;

    if (pub->mTportStream == NULL || pub->mTportStream->mStream == NULL)
    {
        /*
         * Take and release lock to synchronize AddStream return being copied
         * into the member.
         */
        sbfMutex_lock (&tport->mStreamsLock);
        sbfMutex_unlock (&tport->mStreamsLock);
    }
    return pub->mTportStream;
}

static SBF_INLINE void
sbfPubFree (sbfPub pub)
{
    free (pub->mHeader);

    sbfTopic_destroy (pub->mTopic);
    free (pub);
}

static void
sbfPubRemoveEventCb (int fd, short events, void* closure)
{
    sbfPub         pub0 = closure;
    sbfTport       tport = pub0->mTport;
    sbfTportStream tstream = pub0->mTportStream;
    sbfTportTopic  ttopic = pub0->mTportTopic;

    /*
     * If the pub is waiting for the stream to be added, the remove event could
     * be fired before it is ready. If so, defer the remove until the add
     * stream complete callback. The flag is set so the later callback knows
     * this callback has already fired and isn't queued behind it.
     */
    if (!pub0->mReady)
    {
        pub0->mRemoved = 1;
        sbfLog_debug (tport->mLog,
                      "not removing publisher %p (%s); not ready",
                      pub0,
                      sbfTopic_getTopic (pub0->mTopic));
        return;
    }

    sbfLog_debug (tport->mLog,
                  "removing publisher %p (%s) (stream %p)",
                  pub0,
                  sbfTopic_getTopic (pub0->mTopic),
                  tstream);

    TAILQ_REMOVE (&ttopic->mPubs, pub0, mEntry);
    if (TAILQ_EMPTY (&ttopic->mPubs) && sbfTport_removeTopic (tstream, ttopic))
    {
        sbfMutex_lock (&tport->mStreamsLock);
        /*
         * Reference count could be bumped between removeTopic (outside the
         * lock) and here.
         */
        if (sbfRefCount_get (&tstream->mRefCount) == 0)
            sbfTport_removeStream (tport, tstream);
        sbfMutex_unlock (&tport->mStreamsLock);
    }

    if (sbfRefCount_decrement (&pub0->mRefCount))
        sbfPubFree (pub0);
}

static void
sbfPubAddEventCb (int fd, short events, void* closure)
{
    sbfPub         pub0 = closure;
    sbfTopic       topic = pub0->mTopic;
    sbfTportStream tstream = sbfPubEnsureStream (pub0);
    sbfTportTopic  ttopic;

    sbfLog_debug (pub0->mTport->mLog,
                  "adding publisher %p (%s)",
                  pub0,
                  sbfTopic_getTopic (pub0->mTopic));

    ttopic = sbfTport_findTopic (tstream, sbfTopic_getTopic (topic));
    if (ttopic == NULL)
        ttopic = sbfTport_addTopic (tstream, sbfTopic_getTopic (topic));
    sbfLog_debug (pub0->mTport->mLog,
                  "using topic %p (stream %p) for publisher %p (%s)",
                  ttopic,
                  tstream,
                  pub0,
                  sbfTopic_getTopic (pub0->mTopic));

    TAILQ_INSERT_TAIL (&ttopic->mPubs, pub0, mEntry);
    pub0->mTportTopic = ttopic;

    /*
     * Can now remove the reference used to hold the stream while waiting for
     * the event to fire - it's been replaced by the reference added in
     * sbfTport_addTopic.
     */
    sbfRefCount_decrement (&tstream->mRefCount);

    if (tstream->mReady)
        sbfPub_ready (pub0);
}

static void
sbfPubAddStreamCompleteCb (sbfHandlerHandle handle,
                           sbfError error,
                           void* closure)
{
    sbfPub         pub0 = closure;
    sbfTportStream tstream = sbfPubEnsureStream (pub0);
    sbfTportTopic  ttopic;
    sbfPub         pub;

    if (error != 0)
    {
        /*
         * If stream add fails, just return - pubs on this stream will never
         * become ready.
         */
        sbfLog_err (pub0->mTport->mLog,
                    "failed to add stream %p for publisher %p (%s): %s",
                    tstream,
                    pub0,
                    sbfTopic_getTopic (pub0->mTopic),
                    strerror (error));
        return;
    }

    sbfLog_debug (pub0->mTport->mLog,
                  "stream %p is ready (for publisher %p (%s)): thread %u",
                  tstream,
                  pub0,
                  sbfTopic_getTopic (pub0->mTopic),
                  sbfMw_getThreadIndex (tstream->mThread));

    tstream->mReady = 1;

    RB_FOREACH (ttopic, sbfTportTopicTreeImpl, &tstream->mTopics)
    {
        /*
         * If the pub is on this list, we are sure the add callback has been
         * fired. The pub has not been made ready and the remove callback may
         * have been deferred.
         */
        TAILQ_FOREACH (pub, &ttopic->mPubs, mEntry)
        {
            sbfPub_ready (pub);
            if (pub->mRemoved)
                sbfPubRemoveEventCb (-1, 0, pub);
        }
    }

    /*
     * There is one special pub that created the stream - it needs its add
     * callback fired. If the remove has been deferred it needs to be fired
     * too. It may seem silly to call add followed immediately by remove but
     * this shouldn't happen often and ensures that the stream is removed too
     * if necessary.
     */
    sbfPubAddEventCb (-1, 0, pub0);
    if (pub0->mRemoved)
        sbfPubRemoveEventCb (-1, 0, pub0);
}

static void
sbfPubSetStream (sbfPub pub)
{
    sbfTport       tport = pub->mTport;
    sbfTportStream tstream;

    sbfMutex_lock (&tport->mStreamsLock);

    tstream = tport->mHandlerTable->mFindStream (tport->mHandler, pub->mTopic);
    if (tstream != NULL)
    {
        /* We know the thread so can queue the add callback directly. */
        sbfMw_enqueueThread (tstream->mThread,
                             &pub->mEventAdd,
                             sbfPubAddEventCb,
                             pub);
    }
    else
    {
        /*
         * We don't know the thread so we need to create the stream first. The
         * AddStreamComplete callback fires on the right thread and finishes
         * the work on this pub.
         */
        tstream = sbfTport_addStream (tport,
                                      pub->mTopic,
                                      sbfPubAddStreamCompleteCb,
                                      pub);
    }

    /*
     * Take a reference to the stream to prevent it going away even if the
     * number of topics hits zero.
     */
    pub->mTportStream = tstream;
    sbfRefCount_increment (&tstream->mRefCount);

    sbfMutex_unlock (&tport->mStreamsLock);
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
    sbfLog_debug (pub->mTport->mLog,
                  "publisher %p (%s) is ready",
                  pub,
                  sbfTopic_getTopic (pub->mTopic));
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

    sbfLog_debug (tport->mLog,
                  "creating publisher %p (%s)",
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

    sbfLog_debug (pub->mTport->mLog,
                  "queueing remove of publisher %p (%s)",
                  pub,
                  sbfTopic_getTopic (pub->mTopic));

    sbfMw_enqueueThread (pub->mTportStream->mThread,
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
    sbfTportHeader* hdr;

    if (SBF_UNLIKELY (pub->mDestroyed || !pub->mReady))
        return sbfBuffer_new (NULL, size);

    if (SBF_UNLIKELY (size > tport->mHandlerTable->mPacketSize))
    {
        /*
         * This will need to be fragmented (copied) - no point in filling
         * header now.
         */
        return sbfBuffer_new (NULL, size);
    }
    buffer = sbfTport_getBuffer (tstream, pub->mHeaderSize + size);

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
    sbfTport       tport = pub->mTport;
    sbfTportStream tstream = pub->mTportStream;
    sbfTportHeader hdr;
    size_t         size;

    SBF_ASSERT (sbfRefCount_get (&buffer->mRefCount) == 1);

    if (!sbfTport_checkMessageSize (pub, sbfBuffer_getSize (buffer)))
    {
        sbfBuffer_destroy (buffer);
        return;
    }
    if (SBF_UNLIKELY (pub->mDestroyed || !pub->mReady))
    {
        sbfBuffer_destroy (buffer);
        return;
    }

    size = sbfBuffer_getSize (buffer) + pub->mHeaderSize;
    if (SBF_UNLIKELY (size > tport->mHandlerTable->mPacketSize))
    {
        sbfTport_fragment (tstream, buffer, pub->mHeader, pub->mHeaderSize);
        sbfBuffer_destroy (buffer);
        return;
    }

    if (SBF_UNLIKELY (buffer->mOwner != pub))
    {
        sbfPub_send (pub,
                     sbfBuffer_getData (buffer),
                     sbfBuffer_getSize (buffer));
        sbfBuffer_destroy (buffer);
        return;
    }

    hdr = (void*)((char*)sbfBuffer_getData (buffer) - pub->mHeaderSize);
    hdr->mSize = sbfBuffer_getSize (buffer);
    hdr->mFlags = SBF_MESSAGE_FLAG_LAST_IN_PACKET;

    sbfBuffer_setData (buffer, hdr);
    sbfBuffer_setSize (buffer, size);

    sbfTport_sendBuffer (tstream, buffer);
    sbfBuffer_destroy (buffer);
}

void
sbfPub_send (sbfPub pub, void* data, size_t size)
{
    sbfBuffer buffer;

    buffer = sbfPub_getBuffer (pub, size);
    memcpy (sbfBuffer_getData (buffer), data, size);

    sbfPub_sendBuffer (pub, buffer);
}

sbfTopic
sbfPub_getTopic (sbfPub pub)
{
    return pub->mTopic;
}
