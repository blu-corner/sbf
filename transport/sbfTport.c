#include "sbfTport.h"
#include "sbfTportPrivate.h"

typedef struct
{
    sbfTport       mTport;
    sbfTportStream mTportStream;

    sbfMwThread    mThread;
    struct event   mEvent;
} sbfTportDestroyStreamClosure;

static void
sbfTportSecondDestroyStreamCb (int fd, short events, void* closure)
{
    sbfTportDestroyStreamClosure* closure0 = closure;
    sbfTport                      tport = closure0->mTport;

    sbfLog_debug (tport->mLog,
                  "second destroy callback for stream %p",
                  closure0->mTportStream);

    sbfMutex_lock (&tport->mStreamsLock);
    tport->mStreamsWaiting--;
    sbfCondVar_broadcast (&tport->mStreamsCondVar);
    sbfMutex_unlock (&tport->mStreamsLock);

    free (closure0);
}

static void
sbfTportFirstDestroyStreamCb (int fd, short events, void* closure)
{
    sbfTportDestroyStreamClosure* closure0 = closure;
    sbfTportStream                tstream;
    sbfTport                      tport = closure0->mTport;
    sbfTportTopic                 ttopic;
    sbfTportTopic                 ttopic1;
    sbfPub                        pub;
    sbfSub                        sub;

    sbfLog_debug (tport->mLog,
                  "first destroy callback for stream %p",
                  closure0->mTportStream);

    /*
     * Removing pubs or subs may already have disposed of the stream so check
     * if it's still on the transport's list.
     */
    TAILQ_FOREACH (tstream, &tport->mStreams, mEntry)
    {
        if (tstream == closure0->mTportStream)
            break;
    }

    if (tstream == NULL)
    {
        sbfLog_debug (tport->mLog,
                      "stream %p has already been destroyed",
                      closure0->mTportStream);
        sbfTportSecondDestroyStreamCb (fd, events, closure);
        return;
    }

    RB_FOREACH_SAFE (ttopic, sbfTportTopicTreeImpl, &tstream->mTopics, ttopic1)
    {
        TAILQ_FOREACH (pub, &ttopic->mPubs, mEntry)
            sbfPub_destroy (pub);
        TAILQ_FOREACH (sub, &ttopic->mSubs, mEntry)
            sbfSub_destroy (sub);
    }

    /*
     * Queue to wakeup after the publisher and subscriber events (which will
     * have freed and removed the stream).
     */
    sbfMw_enqueueThread (closure0->mThread,
                         &closure0->mEvent,
                         sbfTportSecondDestroyStreamCb,
                         closure0);
}

static void
sbfTportFree (sbfTport tport)
{
    u_int i;

    sbfCondVar_destroy (&tport->mStreamsCondVar);
    sbfMutex_destroy (&tport->mStreamsLock);

    for (i = 0; i < tport->mWeightsListSize; i++)
        regfree (&tport->mWeightsList[i].mPattern);
    free (tport->mWeightsList);
    sbfMutex_destroy (&tport->mWeightsLock);

    sbfKeyValue_destroy (tport->mProperties);

    free ((void*)tport->mName);
    free (tport);
}

static int
sbfTportTryHandler (sbfTport tport, const char* type)
{
    sbfHandlerTable* table;
    sbfHandler       handler;

    sbfLog_debug (tport->mLog, "trying handler %s", type);

    table = sbfHandler_load (tport->mLog, type);
    if (table == NULL)
        return 0;

    if (table->mPacketSize <= sizeof (sbfTportHeader) + SBF_TOPIC_SIZE_LIMIT)
    {
        sbfLog_err (tport->mLog,
                    "handler %s message size is too small (%hu <= %zu)",
                    type,
                    table->mPacketSize,
                    sizeof (sbfTportHeader) + SBF_TOPIC_SIZE_LIMIT);
        return 0;
    }

    handler = table->mCreate (tport, tport->mProperties);
    if (handler == NULL)
    {
        sbfLog_err (tport->mLog, "handler %s create function failed", type);
        return 0;
    }

    tport->mHandlerTable = table;
    tport->mHandler = handler;
    return 1;
}

sbfTport
sbfTport_create (sbfMw mw, const char* name, uint64_t mask)
{
    sbfTport    tport;
    sbfKeyValue properties;
    const char* type;
    char*       copy;
    char*       cp;

    properties = sbfProperties_filterN (sbfMw_getProperties (mw),
                                        "transport",
                                        name,
                                        NULL);

    type = sbfKeyValue_get (properties, "type");
    if (type == NULL)
    {
        sbfLog_err (sbfMw_getLog (mw), "no transport type in properties");
        sbfKeyValue_destroy (properties);
        return NULL;
    }
    copy = xstrdup (type);

    tport = xcalloc (1, sizeof *tport);
    tport->mName = xstrdup (name);
    tport->mLog = sbfMw_getLog (mw);
    tport->mProperties = properties;
    tport->mMw = mw;

    sbfLog_debug (tport->mLog, "creating transport %p: name %s", tport, name);

    if (mask == 0)
        mask = SBF_MW_ALL_THREADS;
    tport->mThreads = mask;

    sbfMutex_init (&tport->mWeightsLock, 0);
    sbfTport_parseWeights (tport);

    TAILQ_INIT (&tport->mStreams);
    sbfMutex_init (&tport->mStreamsLock, 0);
    sbfCondVar_init (&tport->mStreamsCondVar);

    cp = copy;
    while ((type = strsep (&cp, ",")) != NULL)
    {
        if (sbfTportTryHandler (tport, type))
            break;
    }
    if (tport->mHandler == NULL)
    {
        sbfLog_err (tport->mLog, "no handlers available, tried %s", copy);
        free (copy);
        goto fail;
    }
    free (copy);

    return tport;

fail:
    sbfTportFree (tport);
    return NULL;
}

void
sbfTport_destroy (sbfTport tport)
{
    sbfTportStream                tstream;
    sbfTportDestroyStreamClosure* closure0;

    sbfLog_debug (tport->mLog, "destroying transport %p", tport);

    sbfMutex_lock (&tport->mStreamsLock);
    TAILQ_FOREACH (tstream, &tport->mStreams, mEntry)
    {
        closure0 = xmalloc (sizeof *closure0);
        closure0->mTport = tport;
        closure0->mTportStream = tstream;

        closure0->mThread = tstream->mThread;
        sbfMw_enqueueThread (closure0->mThread,
                             &closure0->mEvent,
                             sbfTportFirstDestroyStreamCb,
                             closure0);

        tport->mStreamsWaiting++;
    }
    while (tport->mStreamsWaiting != 0)
    {
        sbfLog_debug (tport->mLog,
                      "transport %p waiting for %u streams",
                      tport,
                      tport->mStreamsWaiting);
        sbfCondVar_wait (&tport->mStreamsCondVar, &tport->mStreamsLock);
    }
    sbfMutex_unlock (&tport->mStreamsLock);

    tport->mHandlerTable->mDestroy (tport->mHandler);

    sbfLog_debug (tport->mLog, "freeing transport %p", tport);
    sbfTportFree (tport);
}

const char*
sbfTport_getName (sbfTport tport)
{
    return tport->mName;
}

sbfMw
sbfTport_getMw (sbfTport tport)
{
    return tport->mMw;
}
