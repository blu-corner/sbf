#ifndef _SBF_TPORT_PRIVATE_H_
#define _SBF_TPORT_PRIVATE_H_

#include "sbfHandlerInternal.h"
#include "sbfMwInternal.h"
#include "sbfPubPrivate.h"
#include "sbfSubPrivate.h"
#include "sbfTopicPrivate.h"

SBF_BEGIN_DECLS

struct sbfTportStreamImpl;

/*
 * Header is followed immediately by zero-terminated topic. The whole header
 * plus topic is padded to eight byte boundary.
 */
typedef SBF_PACKED (struct sbfTportHeaderImpl
{
    uint16_t mSize;
    uint8_t  mTopicSize;
}) sbfTportHeader;

#define SBF_MESSAGE_SIZE_LIMIT \
    (65536 - (sizeof (sbfTportHeader)) - SBF_TOPIC_SIZE_LIMIT)

struct sbfTportTopicImpl
{
    const char*                  mTopic;

    TAILQ_HEAD (, sbfPubImpl)    mPubs;  /* event thread only */
    TAILQ_HEAD (, sbfSubImpl)    mSubs;  /* event thread only */

    sbfSub                       mNext;

    RB_ENTRY (sbfTportTopicImpl) mTreeEntry;
};
typedef struct sbfTportTopicImpl* sbfTportTopic;

RB_HEAD (sbfTportTopicTreeImpl, sbfTportTopicImpl);
typedef struct sbfTportTopicTreeImpl sbfTportTopicTree;

static inline int
sbfTportTopicCmp (sbfTportTopic lhs, sbfTportTopic rhs)
{
    return strcmp (lhs->mTopic, rhs->mTopic);
}
RB_GENERATE_STATIC (sbfTportTopicTreeImpl,
                    sbfTportTopicImpl,
                    mTreeEntry,
                    sbfTportTopicCmp)

struct sbfTportStreamImpl
{
    sbfTport                         mTport;
    sbfMwThread                      mThread;

    sbfHandlerStream                 mStream;

    int                              mReady;  /* event thread only */
    sbfTportTopicTree                mTopics; /* event thread only */

    pthread_mutex_t                  mSendLock;

    TAILQ_ENTRY (sbfTportStreamImpl) mEntry;
};
typedef struct sbfTportStreamImpl* sbfTportStream;

struct sbfTportImpl
{
    sbfMw                             mMw;

    sbfKeyValue                       mProperties;

    sbfHandlerTable*                  mHandlerTable;
    sbfHandler                        mHandler;

    uint64_t                          mThreadMask;

    u_int                             mNextThread;
    pthread_mutex_t                   mNextThreadLock;

    TAILQ_HEAD (, sbfTportStreamImpl) mStreams;
    pthread_mutex_t                   mStreamsLock;
    pthread_cond_t                    mStreamsCond;
};

static inline sbfMwThread
sbfTport_nextThread (sbfTport tport)
{
    sbfMwThread thread;

    pthread_mutex_lock (&tport->mNextThreadLock);
    do
    {
        thread = sbfMw_getThread (tport->mMw, tport->mNextThread++);
        if (tport->mNextThread >= sbfMw_getNumThreads (tport->mMw))
            tport->mNextThread = 0;
    }
    while (~tport->mThreadMask & sbfMw_getThreadMask (thread));
    pthread_mutex_unlock (&tport->mNextThreadLock);

    return thread;
}

static inline sbfTportTopic
sbfTport_findTopic (sbfTportStream tstream, const char* topic)
{
    struct sbfTportTopicImpl impl;

    impl.mTopic = topic;
    return RB_FIND (sbfTportTopicTreeImpl, &tstream->mTopics, &impl);
}

static inline sbfTportTopic
sbfTport_addTopic (sbfTportStream tstream, const char* topic)
{
    sbfTportTopic ttopic;

    ttopic = xcalloc (1, sizeof *ttopic);
    ttopic->mTopic = xstrdup (topic);

    sbfLog_debug ("adding topic %p", ttopic);

    TAILQ_INIT (&ttopic->mPubs);
    TAILQ_INIT (&ttopic->mSubs);

    RB_INSERT (sbfTportTopicTreeImpl, &tstream->mTopics, ttopic);
    return ttopic;
}

static inline int
sbfTport_removeTopic (sbfTportStream tstream, sbfTportTopic ttopic)
{
    sbfLog_debug ("removing topic %p", ttopic);

    RB_REMOVE (sbfTportTopicTreeImpl, &tstream->mTopics, ttopic);

    free ((void*)ttopic->mTopic);
    free (ttopic);

    return RB_EMPTY (&tstream->mTopics);
}

static inline sbfTportStream
sbfTport_addStream (sbfTport tport,
                    sbfTopic topic,
                    sbfHandlerAddStreamCompleteCb cb,
                    void* closure)
{
    sbfTportStream tstream;

    tstream = xcalloc (1, sizeof *tstream);
    tstream->mTport = tport;
    tstream->mThread = sbfTport_nextThread (tport);

    sbfLog_debug ("adding stream %p", tstream);

    tstream->mReady = 0;
    RB_INIT (&tstream->mTopics);

    pthread_mutex_init (&tstream->mSendLock, NULL);

    tstream->mStream = tport->mHandlerTable->mAddStream (tport->mHandler,
                                                         topic,
                                                         tstream->mThread,
                                                         tstream,
                                                         cb,
                                                         closure);

    TAILQ_INSERT_TAIL (&tport->mStreams, tstream, mEntry);
    return tstream;
}

static inline void
sbfTport_removeStream (sbfTport tport, sbfTportStream tstream)
{
    sbfLog_debug ("removing stream %p", tstream);

    TAILQ_REMOVE (&tport->mStreams, tstream, mEntry);

    tport->mHandlerTable->mRemoveStream (tstream->mStream);

    pthread_mutex_destroy (&tstream->mSendLock);
    free (tstream);
}

SBF_END_DECLS
SBF_END_DECLS

#endif /* _SBF_TPORT_PRIVATE_H_ */
