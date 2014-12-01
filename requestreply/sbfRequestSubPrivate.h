#ifndef _SBF_REQUEST_SUB_PRIVATE_H_
#define _SBF_REQUEST_SUB_PRIVATE_H_

SBF_BEGIN_DECLS

struct sbfRequestSubImpl
{
    sbfLog                 mLog;
    const char*            mTopic;

    sbfRequestSubReadyCb   mReadyCb;
    sbfRequestSubRequestCb mRequestCb;
    void*                  mClosure;

    sbfPub                 mPub;
    int                    mPubReady;
    sbfSub                 mSub;
    int                    mSubReady;

    sbfGuid                mNext;
    sbfRequestTree         mRequests;
};

SBF_END_DECLS

#endif /* _SBF_REQUEST_SUB_PRIVATE_H_ */
