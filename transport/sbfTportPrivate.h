#ifndef _SBF_TPORT_PRIVATE_H_
#define _SBF_TPORT_PRIVATE_H_

#include "sbfBatchPrivate.h"
#include "sbfHandler.h"
#include "sbfKeyValue.h"
#include "sbfProperties.h"
#include "sbfPubPrivate.h"
#include "sbfSubPrivate.h"
#include "sbfTopicPrivate.h"

SBF_BEGIN_DECLS

struct sbfTportStreamImpl;

/*
 * Header is followed immediately by zero-terminated topic. The whole header
 * plus topic is padded to eight byte boundary. Each message or fragment has
 * one header, there may multiple messages or fragments in a packet.
 */
SBF_PACKED (struct sbfTportHeaderImpl
{
    uint16_t mSize;      /* this message or fragment size, without header */
    uint8_t  mFragNum;   /* not zero if this is a fragment */
    uint8_t  mTopicSize;
    uint8_t  mFlags;
});
typedef struct sbfTportHeaderImpl* sbfTportHeader;
#define SBF_MESSAGE_FLAG_LAST_IN_PACKET 0x1
#define SBF_MESSAGE_FLAG_LAST_FRAGMENT 0x2

/*
 * Maximum message size; may be fragmented according to handler packet size.
 * This is an arbitrary limit, the real limit is about 256 * 65536.
 */
#define SBF_MESSAGE_SIZE_LIMIT 65536

struct sbfTportTopicImpl
{
    const char*                  mTopic;

    TAILQ_HEAD (, sbfPubImpl)    mPubs;  /* event thread only */
    TAILQ_HEAD (, sbfSubImpl)    mSubs;  /* event thread only */

    sbfSub                       mNext;

    u_int                        mFragNum;
    size_t                       mFragSize;
    void*                        mFragData;
    size_t                       mFragAllocated;

    RB_ENTRY (sbfTportTopicImpl) mTreeEntry;
};
typedef struct sbfTportTopicImpl* sbfTportTopic;

SBF_RB_TREE (sbfTportTopic, Tree, mTreeEntry,
{
    return strcmp (lhs->mTopic, rhs->mTopic);
})

struct sbfTportStreamImpl
{
    sbfTport                         mTport;
    sbfMwThread                      mThread;

    sbfHandlerStream                 mStream;

    sbfRefCount                      mRefCount;
    int                              mReady;  /* event thread only */
    sbfTportTopicTree                mTopics; /* event thread only */

    sbfMutex                         mSendLock;

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
    const char*                       mName;
    sbfLog                            mLog;
    sbfMw                             mMw;

    sbfKeyValue                       mProperties;

    sbfHandlerTable*                  mHandlerTable;
    sbfHandler                        mHandler;

    uint64_t                          mThreads;

    u_int                             mWeights[SBF_MW_THREAD_LIMIT];
    sbfMutex                          mWeightsLock;
    u_int                             mWeightsListSize;
    sbfTportWeight*                   mWeightsList;

    TAILQ_HEAD (, sbfTportStreamImpl) mStreams;
    sbfMutex                          mStreamsLock;
    sbfCondVar                        mStreamsCondVar;
    u_int                             mStreamsWaiting;
};

void sbfTport_parseWeights (sbfTport tport);
void sbfTport_adjustWeight (sbfTport tport, sbfMwThread thread, int change);
u_int sbfTport_topicWeight (sbfTport tport, sbfTopic topic);
sbfMwThread sbfTport_nextThread (sbfTport tport);

void sbfTport_fragment (sbfTportStream tstream,
                        sbfBuffer buffer,
                        sbfTportHeader hdr,
                        size_t hdrSize);
sbfBuffer sbfTport_assemble (sbfTportStream tstream,
                             sbfTportTopic ttopic,
                             sbfTportHeader hdr);

static SBF_INLINE int
sbfTport_checkMessageSize (sbfPub pub, size_t size)
{
    sbfTport tport = pub->mTport;

    if (SBF_UNLIKELY (size > SBF_MESSAGE_SIZE_LIMIT))
    {
        sbfLog_err (tport->mLog,
                    "message size %zu too big on publisher %p (%s)",
                    size,
                    pub,
                    sbfTopic_getTopic (pub->mTopic));
        return 0;
    }
    return 1;
}

static SBF_INLINE sbfBuffer
sbfTport_getBuffer (sbfTportStream tstream, size_t size)
{
    return tstream->mTport->mHandlerTable->mGetBuffer (tstream->mStream, size);
}

static SBF_INLINE void
sbfTport_sendBufferLocked (sbfTportStream tstream, sbfBuffer buffer)
{
    sbfTport tport = tstream->mTport;

    tport->mHandlerTable->mSendBuffer (tstream->mStream, buffer);
}

static SBF_INLINE void
sbfTport_sendBuffer (sbfTportStream tstream, sbfBuffer buffer)
{
    sbfMutex_lock (&tstream->mSendLock);
    sbfTport_sendBufferLocked (tstream, buffer);
    sbfMutex_unlock (&tstream->mSendLock);
}

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

    sbfLog_debug (tstream->mTport->mLog, "adding topic %p", ttopic);

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

    sbfLog_debug (tstream->mTport->mLog, "removing topic %p", ttopic);

    empty = sbfRefCount_decrement (&tstream->mRefCount);
    RB_REMOVE (sbfTportTopicTreeImpl, &tstream->mTopics, ttopic);
    SBF_ASSERT (!empty || RB_EMPTY (&tstream->mTopics));

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

    sbfLog_debug (tport->mLog, "adding stream %p", tstream);

    sbfRefCount_init (&tstream->mRefCount, 0);

    tstream->mReady = 0;
    RB_INIT (&tstream->mTopics);

    sbfMutex_init (&tstream->mSendLock, 0);

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
    sbfLog_debug (tport->mLog, "removing stream %p", tstream);

    TAILQ_REMOVE (&tport->mStreams, tstream, mEntry);

    tport->mHandlerTable->mRemoveStream (tstream->mStream);

    sbfMutex_destroy (&tstream->mSendLock);
    free (tstream);
}

SBF_END_DECLS

#endif /* _SBF_TPORT_PRIVATE_H_ */
