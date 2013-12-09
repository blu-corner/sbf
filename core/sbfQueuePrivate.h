#ifndef _SBF_QUEUE_PRIVATE_H_
#define _SBF_QUEUE_PRIVATE_H_

#include "sbfMw.h"
#include "sbfMwInternal.h"
#include "sbfMwPrivate.h"
#include "sbfPool.h"
#include "sbfRefCount.h"

SBF_BEGIN_DECLS

#define SBF_QUEUE_ITEM_DATA_SIZE 32

struct sbfQueueItemImpl
{
    sbfQueueCb                     mCb;
    void*                          mClosure;

    char                           mData[SBF_QUEUE_ITEM_DATA_SIZE];

    TAILQ_ENTRY (sbfQueueItemImpl) mEntry;
};

struct sbfQueueImpl
{
    pthread_mutex_t                 mMutex;
    pthread_cond_t                  mCond;

    int                             mDestroyed;
    sbfRefCount                     mRefCount;

    sbfPool                         mPool;
    TAILQ_HEAD (, sbfQueueItemImpl) mItems;
};

void sbfQueue_addRef (sbfQueue queue);
void sbfQueue_removeRef (sbfQueue queue);

SBF_END_DECLS

#endif /* _SBF_QUEUE_H_ */
