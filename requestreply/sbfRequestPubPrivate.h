#ifndef _SBF_REQUEST_PUB_PRIVATE_H_
#define _SBF_REQUEST_PUB_PRIVATE_H_

SBF_BEGIN_DECLS

struct sbfRequestPubImpl
{
    sbfRequestPubReadyCb  mReadyCb;
    void*                 mClosure;

    sbfPub                mPub;
    int                   mPubReady;
    sbfSub                mSub;
    int                   mSubReady;

    sbfGuid               mNext;
    sbfRequestTree        mRequests;
};

SBF_END_DECLS

#endif /* _SBF_REQUEST_PUB_PRIVATE_H_ */
