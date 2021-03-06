#include "sbfTport.h"
#include "sbfTportPrivate.h"

static SBF_INLINE void
sbfSubFree (sbfSub sub)
{
    sbfTopic_destroy (sub->mTopic);
    free (sub);
}

static SBF_INLINE sbfTportStream
sbfSubEnsureStream (sbfSub sub)
{
    sbfTport tport = sub->mTport;

    if (sub->mTportStream == NULL || sub->mTportStream->mStream == NULL)
    {
        /*
         * Take and release lock to synchronize AddStream return being copied
         * into the member.
         */
        sbfMutex_lock (&tport->mStreamsLock);
        sbfMutex_unlock (&tport->mStreamsLock);
    }
    return sub->mTportStream;
}

static void
sbfSubRemoveEventCb (int fd, short events, void* closure)
{
    sbfSub         sub0 = closure;
    sbfTport       tport = sub0->mTport;
    sbfTportStream tstream = sub0->mTportStream;
    sbfTportTopic  ttopic = sub0->mTportTopic;

    /*
     * If the sub is waiting for the stream to be added, the remove event could
     * be fired before it is ready. If so, defer the remove until the add
     * stream complete callback. The flag is set so the later callback knows
     * this callback has already fired and isn't queued behind it.
     */
    if (!sub0->mReady)
    {
        sub0->mRemoved = 1;
        sbfLog_debug (tport->mLog,
                      "not removing subscription %p (%s); not ready",
                      sub0,
                      sbfTopic_getTopic (sub0->mTopic));
        return;
    }

    sbfLog_debug (tport->mLog,
                  "removing subscription %p (%s) (stream %p)",
                  sub0,
                  sbfTopic_getTopic (sub0->mTopic),
                  tstream);

    sbfTport_adjustWeight (tport, tstream->mThread, -(int)sub0->mWeight);

    if (ttopic->mNext == sub0)
        ttopic->mNext = TAILQ_NEXT (sub0, mEntry);
    TAILQ_REMOVE (&ttopic->mSubs, sub0, mEntry);
    if (TAILQ_EMPTY (&ttopic->mSubs) && sbfTport_removeTopic (tstream, ttopic))
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

    if (sbfRefCount_decrement (&sub0->mRefCount))
        sbfSubFree (sub0);
}

static void
sbfSubAddEventCb (int fd, short events, void* closure)
{
    sbfSub         sub0 = closure;
    sbfTopic       topic = sub0->mTopic;
    sbfTportStream tstream = sbfSubEnsureStream (sub0);
    sbfTportTopic  ttopic;

    sbfLog_debug (sub0->mTport->mLog,
                  "adding subscription %p (%s)",
                  sub0,
                  sbfTopic_getTopic (sub0->mTopic));

    ttopic = sbfTport_findTopic (tstream, sbfTopic_getTopic (topic));
    if (ttopic == NULL)
        ttopic = sbfTport_addTopic (tstream, sbfTopic_getTopic (topic));
    sbfLog_debug (sub0->mTport->mLog,
                  "using topic %p (stream %p) for subscription %p (%s)",
                  ttopic,
                  tstream,
                  sub0,
                  sbfTopic_getTopic (sub0->mTopic));

    TAILQ_INSERT_TAIL (&ttopic->mSubs, sub0, mEntry);
    sub0->mTportTopic = ttopic;

    /*
     * Can now remove the reference used to hold the stream while waiting for
     * the event to fire - it's been replaced by the reference added in
     * sbfTport_addTopic.
     */
    sbfRefCount_decrement (&tstream->mRefCount);

    if (tstream->mReady)
        sbfSub_ready (sub0);
}

static void
sbfSubAddStreamCompleteCb (sbfHandlerHandle handle,
                           sbfError error,
                           void* closure)
{
    sbfSub         sub0 = closure;
    sbfTportStream tstream = sbfSubEnsureStream (sub0);
    sbfTportTopic  ttopic;
    sbfSub         sub;

    if (error != 0)
    {
        /*
         * If stream add fails, just return - subs on this stream will never
         * become ready.
         */
        sbfLog_err (sub0->mTport->mLog,
                    "failed to add stream %p for subscription %p (%s): %s",
                    tstream,
                    sub0,
                    sbfTopic_getTopic (sub0->mTopic),
                    strerror (error));
        return;
    }

    sbfLog_debug (sub0->mTport->mLog,
                  "stream %p is ready (for subscription %p (%s)): thread %u",
                  tstream,
                  sub0,
                  sbfTopic_getTopic (sub0->mTopic),
                  sbfMw_getThreadIndex (tstream->mThread));

    tstream->mReady = 1;

    RB_FOREACH (ttopic, sbfTportTopicTreeImpl, &tstream->mTopics)
    {
        /*
         * If the sub is on this list, we are sure the add callback has been
         * fired. The sub has not been made ready and the remove callback may
         * have been deferred.
         */
        TAILQ_FOREACH (sub, &ttopic->mSubs, mEntry)
        {
            sbfSub_ready (sub);
            if (sub->mRemoved)
                sbfSubRemoveEventCb (-1, 0, sub);
        }
    }

    /*
     * There is one special sub that created the stream - it needs its add
     * callback fired. If the remove has been deferred it needs to be fired
     * too. It may seem silly to call add followed immediately by remove but
     * this shouldn't happen often and ensures that the stream is removed too
     * if necessary.
     */
    sbfSubAddEventCb (-1, 0, sub0);
    if (sub0->mRemoved)
        sbfSubRemoveEventCb (-1, 0, sub0);
}

static void
sbfSubSetStream (sbfSub sub)
{
    sbfTport       tport = sub->mTport;
    sbfTportStream tstream;

    sbfMutex_lock (&tport->mStreamsLock);

    tstream = tport->mHandlerTable->mFindStream (tport->mHandler, sub->mTopic);
    if (tstream != NULL)
    {
        /* We know the thread so can queue the add callback directly. */
        sbfMw_enqueueThread (tstream->mThread,
                             &sub->mEventAdd,
                             sbfSubAddEventCb,
                             sub);
    }
    else
    {
        /*
         * We don't know the thread so we need to create the stream first. The
         * AddStreamComplete callback fires on the right thread and finishes
         * the work on this sub.
         */
        tstream = sbfTport_addStream (tport,
                                      sub->mTopic,
                                      sbfSubAddStreamCompleteCb,
                                      sub);
    }

    /*
     * Take a reference to the stream to prevent it going away even if the
     * number of topics hits zero.
     */
    sub->mTportStream = tstream;
    sbfRefCount_increment (&tstream->mRefCount);

    sbfTport_adjustWeight (tport, tstream->mThread, sub->mWeight);
    sbfMutex_unlock (&tport->mStreamsLock);
}

static void
sbfSubReadyQueueCb (sbfQueueItem item, void* closure)
{
    sbfSub sub = closure;

    if (!sub->mDestroyed)
        sub->mReadyCb (sub, sub->mClosure);

    if (sbfRefCount_decrement (&sub->mRefCount))
        sbfSubFree (sub);
}

void
sbfSub_ready (sbfSub sub)
{
    sbfLog_debug (sub->mTport->mLog,
                  "subscription %p (%s) is ready",
                  sub,
                  sbfTopic_getTopic (sub->mTopic));
    sub->mReady = 1;

    if (!sub->mDestroyed && sub->mReadyCb != NULL)
    {
        sbfRefCount_increment (&sub->mRefCount);
        sbfQueue_enqueue (sub->mQueue, sbfSubReadyQueueCb, sub);
    }
}

static void
sbfSubMessageQueueCb (sbfQueueItem item, void* closure)
{
    sbfSub    sub = closure;
    sbfBuffer buffer = *(sbfBuffer*)sbfQueue_getItemData (item);

    if (!sub->mDestroyed)
        sub->mMessageCb (sub, buffer, sub->mClosure);
    sbfBuffer_destroy (buffer);

    if (sbfRefCount_decrement (&sub->mRefCount))
        sbfSubFree (sub);
}

void
sbfSub_message (sbfSub sub, sbfBuffer buffer)
{
    sbfQueueItem item;

    if (!sub->mDestroyed && sub->mMessageCb != NULL)
    {
        if (sub->mQueueBypass)
            sub->mMessageCb (sub, buffer, sub->mClosure);
        else
        {
            item = sbfQueue_getItem (sub->mQueue, sbfSubMessageQueueCb, sub);

            *(sbfBuffer*)sbfQueue_getItemData (item) = buffer;
            sbfBuffer_addRef (buffer);

            sbfRefCount_increment (&sub->mRefCount);
            sbfQueue_enqueueItem (sub->mQueue, item);
        }
    }
}

sbfSub
sbfSub_create (sbfTport tport,
               sbfQueue queue,
               const char* topic0,
               sbfSubReadyCb readyCb,
               sbfSubMessageCb messageCb,
               void* closure)
{
    sbfSub   sub;
    sbfTopic topic;

    topic = sbfTopic_create (SBF_TOPIC_SUB, topic0);
    if (topic == NULL)
        return NULL;

    sub = xcalloc (1, sizeof *sub);
    sub->mTopic = topic;

    sub->mDestroyed = 0;
    sbfRefCount_init (&sub->mRefCount, 1);

    sub->mReady = 0;

    sub->mTport = tport;
    sub->mTportStream = NULL;
    sub->mTportTopic = NULL;

    sbfLog_debug (tport->mLog,
                  "creating subscription %p (%s)",
                  sub,
                  sbfTopic_getTopic (sub->mTopic));

    sub->mQueue = queue;
    sub->mReadyCb = readyCb;
    sub->mMessageCb = messageCb;
    sub->mClosure = closure;

    sub->mWeight = sbfTport_topicWeight (tport, sub->mTopic);
    sbfSubSetStream (sub);

    return sub;
}

void
sbfSub_destroy (sbfSub sub)
{
    if (sub->mDestroyed)
        return;
    sub->mDestroyed = 1;

    sbfLog_debug (sub->mTport->mLog,
                  "queueing remove of subscription %p (%s)",
                  sub,
                  sbfTopic_getTopic (sub->mTopic));
    sbfMw_enqueueThread (sub->mTportStream->mThread,
                         &sub->mEventRemove,
                         sbfSubRemoveEventCb,
                         sub);
}

sbfTopic
sbfSub_getTopic (sbfSub sub)
{
    return sub->mTopic;
}

void
sbfSub_setQueueBypass (sbfSub sub, int set)
{
    sbfLog_debug (sub->mTport->mLog,
                  "queue bypass %s for subscription %p (%s)",
                  set ? "ON" : "OFF",
                  sub,
                  sbfTopic_getTopic (sub->mTopic));
    sub->mQueueBypass = set;
}
