#include "sbfTport.h"
#include "sbfTportPrivate.h"

static inline void
sbfSubFree (sbfSub sub)
{
    sbfTopic_destroy (sub->mTopic);
    free (sub);
}

static void
sbfSubAddEventCb (int fd, short events, void* closure)
{
    sbfSub         sub0 = closure;
    sbfTopic       topic = sub0->mTopic;
    sbfTportStream tstream = sub0->mTportStream;
    sbfTportTopic  ttopic;

    sbfLog_debug ("adding %p", sub0);

    ttopic = sbfTport_findTopic (tstream, sbfTopic_getTopic (topic));
    if (ttopic == NULL)
        ttopic = sbfTport_addTopic (tstream, sbfTopic_getTopic (topic));
    sbfLog_debug ("using topic %p (stream %p) for %p", ttopic, tstream, sub0);

    TAILQ_INSERT_TAIL (&ttopic->mSubs, sub0, mEntry);
    sub0->mTportTopic = ttopic;

    if (tstream->mReady)
        sbfSub_ready (sub0);
}

static void
sbfSubAddStreamCompleteCb (sbfHandlerHandle handle,
                           sbfError error,
                           void* closure)
{
    sbfSub         sub0 = closure;
    sbfTport       tport = sub0->mTport;
    sbfTportStream tstream;
    sbfTportTopic  ttopic;
    sbfSub         sub;

    if (sub0->mTportStream == NULL || sub0->mTportStream->mStream == NULL)
    {
        /*
         * Take and release lock to synchronize AddStream return being copied
         * into the mStream member.
         */
        pthread_mutex_lock (&tport->mStreamsLock);
        pthread_mutex_unlock (&tport->mStreamsLock);
    }
    tstream = sub0->mTportStream;

    if (error != 0)
    {
        /*
         * If stream add fails, just return - sublishers on this stream will
         * never become ready.
         */
        sbfLog_err ("failed to add stream %p: %s", tstream, strerror (error));
        return;
    }

    sbfLog_debug ("stream %p is ready (for %p): thread %u",
                  tstream,
                  sub0,
                  sbfMw_getThreadIndex (tstream->mThread));

    tstream->mReady = 1;

    RB_FOREACH (ttopic, sbfTportTopicTreeImpl, &tstream->mTopics)
    {
        TAILQ_FOREACH (sub, &ttopic->mSubs, mEntry)
            sbfSub_ready (sub);
    }

    sbfSubAddEventCb (-1, 0, sub0);
}

static void
sbfSubRemoveEventCb (int fd, short events, void* closure)
{
    sbfSub         sub0 = closure;
    sbfTport       tport = sub0->mTport;
    sbfTportTopic  ttopic = sub0->mTportTopic;
    sbfTportStream tstream = sub0->mTportStream;

    sbfLog_debug ("removing %p", sub0);

    TAILQ_REMOVE (&ttopic->mSubs, sub0, mEntry);
    if (TAILQ_EMPTY (&ttopic->mSubs) && sbfTport_removeTopic (tstream, ttopic))
    {
        pthread_mutex_lock (&tport->mStreamsLock);
        sbfTport_removeStream (tport, tstream);
        pthread_mutex_unlock (&tport->mStreamsLock);
    }

    if (sbfRefCount_decrement (&sub0->mRefCount))
        sbfSubFree (sub0);
}

static void
sbfSubSetStream (sbfSub sub)
{
    sbfTport       tport = sub->mTport;
    sbfTportStream tstream;

    pthread_mutex_lock (&tport->mStreamsLock);
    tstream = tport->mHandlerTable->mFindStream (tport->mHandler, sub->mTopic);
    if (tstream != NULL)
    {
        sbfMw_enqueue (tstream->mThread,
                       &sub->mEventAdd,
                       sbfSubAddEventCb,
                       sub);
    }
    else
    {
        sub->mTportStream = sbfTport_addStream (tport,
                                                sub->mTopic,
                                                sbfSubAddStreamCompleteCb,
                                                sub);
    }
    pthread_mutex_unlock (&tport->mStreamsLock);
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
    sbfLog_debug ("%p is ready", sub);
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
        item = sbfQueue_getItem (sub->mQueue, sbfSubMessageQueueCb, sub);

        *(sbfBuffer*)sbfQueue_getItemData (item) = buffer;
        sbfBuffer_addRef (buffer);

        sbfRefCount_increment (&sub->mRefCount);
        sbfQueue_enqueueItem (sub->mQueue, item);
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

    sbfLog_debug ("creating %p: topic %s",
                  sub,
                  sbfTopic_getTopic (sub->mTopic));

    sub->mQueue = queue;
    sub->mReadyCb = readyCb;
    sub->mMessageCb = messageCb;
    sub->mClosure = closure;

    sbfSubSetStream (sub);

    return sub;
}

void
sbfSub_destroy (sbfSub sub)
{
    if (sub->mDestroyed)
        return;
    sub->mDestroyed = 1;

    sbfMw_enqueue (sub->mTportStream->mThread,
                   &sub->mEventRemove,
                   sbfSubRemoveEventCb,
                   sub);
}

sbfTopic
sbfSub_getTopic (sbfSub sub)
{
    return sub->mTopic;
}
