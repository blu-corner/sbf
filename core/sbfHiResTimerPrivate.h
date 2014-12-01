#ifndef _SBF_HI_RES_TIMER_PRIVATE_H_
#define _SBF_HI_RES_TIMER_PRIVATE_H_

#include "sbfMw.h"
#include "sbfMwPrivate.h"
#include "sbfPerfCounter.h"
#include "sbfRefCount.h"

SBF_BEGIN_DECLS

struct sbfHiResTimerImpl
{
    sbfHiResTimerCb              mCb;
    void*                        mClosure;

    u_int                        mInterval;
    uint64_t                     mTicks;
    uint64_t                     mNext;

    sbfQueue                     mQueue;

    int                          mDestroyed;
    int                          mFreed;
    sbfRefCount                  mRefCount;

    RB_ENTRY (sbfHiResTimerImpl) mTreeEntry;
};

SBF_RB_TREE (sbfHiResTimer, Tree, mTreeEntry,
{
    if (lhs->mNext < rhs->mNext)
        return -1;
    if (lhs->mNext > rhs->mNext)
        return 1;
    return 0;
});

void sbfHiResTimer_queueDispatch (sbfQueue queue);
void sbfHiResTimer_queueDestroy (sbfQueue queue);

SBF_END_DECLS

#endif /* _SBF_HI_RES_TIMER_H_ */
