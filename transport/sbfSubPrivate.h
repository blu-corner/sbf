#ifndef _SBF_SUB_PRIVATE_H_
#define _SBF_SUB_PRIVATE_H_

SBF_BEGIN_DECLS

struct sbfTportImpl;
struct sbfTportStreamImpl;
struct sbfTportTopicImpl;
struct sbfTportHeaderImpl;

struct sbfSubImpl
{
    sbfTopic                   mTopic;

    int                        mDestroyed;
    sbfRefCount                mRefCount;

    int                        mReady;

    struct sbfTportImpl*       mTport;
    struct sbfTportStreamImpl* mTportStream;
    struct sbfTportTopicImpl*  mTportTopic;

    sbfQueue                   mQueue;
    sbfSubReadyCb              mReadyCb;
    sbfSubMessageCb            mMessageCb;
    void*                      mClosure;

    struct event               mEventAdd;
    struct event               mEventRemove;

    TAILQ_ENTRY (sbfSubImpl)   mEntry;
};

void sbfSub_ready (sbfSub sub);
void sbfSub_message (sbfSub sub, sbfBuffer buffer);

#endif /* _SBF_SUB_PRIVATE_H_ */
