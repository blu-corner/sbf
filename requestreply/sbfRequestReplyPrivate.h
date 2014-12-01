#ifndef _SBF_REQUEST_REPLY_PRIVATE_H_
#define _SBF_REQUEST_REPLY_PRIVATE_H_

#include "sbfGuid.h"
#include "sbfRefCount.h"

SBF_BEGIN_DECLS

SBF_PACKED (struct sbfRequestHeaderImpl
{
    sbfGuid  mGuid;
    uint64_t mSpace[4];
});

typedef struct sbfRequestHeaderImpl sbfRequestHeader;

struct sbfRequestImpl
{
    /* One of these is set: mPub for a sent request or mSub for a received. */
    sbfRequestPub             mPub;
    sbfRequestSub             mSub;

    sbfGuid                   mGuid;
    sbfRefCount               mRefCount;

    sbfRequestPubReplyCb      mReplyCb;
    void*                     mClosure;

    RB_ENTRY (sbfRequestImpl) mTreeEntry;
};

SBF_RB_TREE (sbfRequest, Tree, mTreeEntry,
{
    return sbfGuid_compare (&lhs->mGuid, &rhs->mGuid);
})

SBF_END_DECLS

#endif /* _SBF_REQUEST_REPLY_PRIVATE_H_ */

