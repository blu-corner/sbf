#ifndef _SBF_PUB_PRIVATE_H_
#define _SBF_PUB_PRIVATE_H_

SBF_BEGIN_DECLS

struct sbfTportImpl;
struct sbfTportStreamImpl;
struct sbfTportTopicImpl;
struct sbfTportHeaderImpl;

struct sbfPubImpl
{
    sbfTopic                   mTopic;

    int                        mDestroyed;
    sbfRefCount                mRefCount;

    int                        mReady;

    struct sbfTportImpl*       mTport;
    struct sbfTportStreamImpl* mTportStream;
    struct sbfTportTopicImpl*  mTportTopic;

    sbfQueue                   mQueue;
    sbfPubReadyCb              mReadyCb;
    void*                      mClosure;

    struct event               mEventAdd;
    struct event               mEventRemove;

    struct sbfTportHeaderImpl* mHeader;
    size_t                     mHeaderSize;

    TAILQ_ENTRY (sbfPubImpl)   mEntry;
};

void sbfPub_ready (sbfPub pub);

SBF_END_DECLS

#endif /* _SBF_PUB_PRIVATE_H_ */
