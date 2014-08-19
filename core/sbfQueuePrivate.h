#ifndef _SBF_QUEUE_PRIVATE_H_
#define _SBF_QUEUE_PRIVATE_H_

#include "sbfMw.h"
#include "sbfMwInternal.h"
#include "sbfMwPrivate.h"
#include "sbfPool.h"
#include "sbfRefCount.h"

SBF_BEGIN_DECLS

#define SBF_QUEUE_ITEM_DATA_SIZE 32
#define SBF_QUEUE_BLOCKING(queue) (!((queue)->mFlags & SBF_QUEUE_NONBLOCK))

#ifdef WIN32
#include "sbfQueueWin32.h"
#else
#include "sbfQueueLinux.h"
#endif

struct sbfQueueItemImpl
{
    sbfQueueCb mCb;
    void*      mClosure;

    char       mData[SBF_QUEUE_ITEM_DATA_SIZE];

    SBF_QUEUE_ITEM_DECL;
};

struct sbfQueueImpl
{
    int         mFlags;

    int         mDestroyed;
    sbfRefCount mRefCount;

    sbfPool     mPool;

    SBF_QUEUE_DECL;
};

#define SBF_QUEUE_FUNCTIONS
#ifdef WIN32
#include "sbfQueueWin32.h"
#else
#include "sbfQueueLinux.h"
#endif
#undef SBF_QUEUE_FUNCTIONS

void sbfQueue_addRef (sbfQueue queue);
void sbfQueue_removeRef (sbfQueue queue);

SBF_END_DECLS

#endif /* _SBF_QUEUE_H_ */
