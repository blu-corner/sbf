#ifndef _SBF_BATCH_PRIVATE_H_
#define _SBF_BATCH_PRIVATE_H_

#include "sbfDeque.h"

SBF_BEGIN_DECLS

struct sbfTportImpl;
struct sbfTportStreamImpl;
struct sbfTportHeaderImpl;

struct sbfBatchItemImpl
{
    struct sbfTportStreamImpl*  mTportStream;

    u_int                       mMessages;
    sbfBuffer                   mCurrent;
    struct sbfTportHeaderImpl*  mLast;
    sbfDeque                    mBuffers;

    RB_ENTRY (sbfBatchItemImpl) mTreeEntry;
};
typedef struct sbfBatchItemImpl* sbfBatchItem;

SBF_RB_TREE (sbfBatchItem, Tree, mTreeEntry,
{
    if ((uintptr_t)lhs->mTportStream < (uintptr_t)rhs->mTportStream)
        return -1;
    if ((uintptr_t)lhs->mTportStream > (uintptr_t)rhs->mTportStream)
        return 1;
    return 0;
})

struct sbfBatchImpl
{
    struct sbfTportImpl* mTport;

    sbfPool              mPool;
    sbfBatchItemTree     mTree;
    sbfMutex             mMutex;
};

SBF_END_DECLS

#endif /* _SBF_BATCH_PRIVATE_H_ */
