#ifndef _SBF_QUEUE_PRIVATE_H_
#define _SBF_QUEUE_PRIVATE_H_

#include "sbfMw.h"
#include "sbfMwPrivate.h"
#include "sbfPool.h"
#include "sbfProperties.h"
#include "sbfRefCount.h"
#include "sbfHiResTimerPrivate.h"

SBF_BEGIN_DECLS

#define SBF_QUEUE_ITEM_DATA_SIZE 32

#define SBF_QUEUE_NONBLOCK 0x1

#define SBF_QUEUE_BLOCKING(queue) \
    (!((queue)->mFlags & SBF_QUEUE_NONBLOCK) && RB_EMPTY (&queue->mHiResTimers))

#if _WIN32
#include "sbfQueueWin32.h"
#elif linux
#include "sbfQueueLinux.h"
#else
#include "sbfQueueDarwin.h"
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
    const char*       mName;
    sbfLog            mLog;
    sbfKeyValue       mProperties;
    int               mFlags;

    int               mExited;
    int               mDestroyed;
    sbfRefCount       mRefCount;

    sbfPool           mPool;

    sbfHiResTimerTree mHiResTimers;

    SBF_QUEUE_DECL;
};

#define SBF_QUEUE_FUNCTIONS
#if _WIN32
#include "sbfQueueWin32.h"
#elif linux
#include "sbfQueueLinux.h"
#else
#include "sbfQueueDarwin.h"
#endif
#undef SBF_QUEUE_FUNCTIONS

void sbfQueue_addRef (sbfQueue queue);
void sbfQueue_removeRef (sbfQueue queue);

SBF_END_DECLS

#endif /* _SBF_QUEUE_H_ */
