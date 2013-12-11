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

static SBF_INLINE int
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

    sbfRefCount                      mRefCount;
    int                              mReady;  /* event thread only */
    sbfTportTopicTree                mTopics; /* event thread only */

    pthread_mutex_t                  mSendLock;

    TAILQ_ENTRY (sbfTportStreamImpl) mEntry;
};
typedef struct sbfTportStreamImpl* sbfTportStream;

typedef struct
{
    u_int   mWeight;
    regex_t mPattern;
} sbfTportWeight;

struct sbfTportImpl
{
    sbfMw                             mMw;

    sbfKeyValue                       mProperties;

    sbfHandlerTable*                  mHandlerTable;
    sbfHandler                        mHandler;

    uint64_t                          mThreads;

    u_int                             mWeights[SBF_MW_THREAD_LIMIT];
    pthread_mutex_t                   mWeightsLock;
    u_int                             mWeightsListSize;
    sbfTportWeight*                   mWeightsList;

    TAILQ_HEAD (, sbfTportStreamImpl) mStreams;
    pthread_mutex_t                   mStreamsLock;
    pthread_cond_t                    mStreamsCond;
};

void sbfTport_parseWeights (sbfTport tport);
void sbfTport_adjustWeight (sbfTport tport, sbfMwThread thread, int change);
u_int sbfTport_topicWeight (sbfTport tport, sbfTopic topic);
sbfMwThread sbfTport_nextThread (sbfTport tport);

static SBF_INLINE sbfTportTopic
sbfTport_findTopic (sbfTportStream tstream, const char* topic)
{
    struct sbfTportTopicImpl impl;

    impl.mTopic = topic;
    return RB_FIND (sbfTportTopicTreeImpl, &tstream->mTopics, &impl);
}

static SBF_INLINE sbfTportTopic
sbfTport_addTopic (sbfTportStream tstream, const char* topic)
{
    sbfTportTopic ttopic;

    ttopic = xcalloc (1, sizeof *ttopic);
    ttopic->mTopic = xstrdup (topic);

    sbfLog_debug ("adding topic %p", ttopic);

    TAILQ_INIT (&ttopic->mPubs);
    TAILQ_INIT (&ttopic->mSubs);

    RB_INSERT (sbfTportTopicTreeImpl, &tstream->mTopics, ttopic);
    sbfRefCount_increment (&tstream->mRefCount);
    return ttopic;
}

static SBF_INLINE int
sbfTport_removeTopic (sbfTportStream tstream, sbfTportTopic ttopic)
{
    u_int empty;

    sbfLog_debug ("removing topic %p", ttopic);

    empty = sbfRefCount_decrement (&tstream->mRefCount);
    RB_REMOVE (sbfTportTopicTreeImpl, &tstream->mTopics, ttopic);
    assert (!empty || RB_EMPTY (&tstream->mTopics));

    free ((void*)ttopic->mTopic);
    free (ttopic);

    return empty;
}

static SBF_INLINE sbfTportStream
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

    sbfRefCount_init (&tstream->mRefCount, 0);

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

static SBF_INLINE void
sbfTport_removeStream (sbfTport tport, sbfTportStream tstream)
{
    sbfLog_debug ("removing stream %p", tstream);

    TAILQ_REMOVE (&tport->mStreams, tstream, mEntry);

    tport->mHandlerTable->mRemoveStream (tstream->mStream);

    pthread_mutex_destroy (&tstream->mSendLock);
    free (tstream);
}

SBF_END_DECLS

#endif /* _SBF_TPORT_PRIVATE_H_ */
