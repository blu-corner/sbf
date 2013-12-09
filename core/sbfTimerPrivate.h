#ifndef _SBF_TIMER_PRIVATE_H_
#define _SBF_TIMER_PRIVATE_H_

#include "sbfMw.h"
#include "sbfMwInternal.h"
#include "sbfMwPrivate.h"
#include "sbfRefCount.h"

SBF_BEGIN_DECLS

struct sbfTimerImpl
{
    sbfTimerCb     mCb;
    void*          mClosure;

    double         mInterval;

    sbfMwThread    mThread;
    sbfQueue       mQueue;

    int            mDestroyed;
    sbfRefCount    mRefCount;

    struct event   mEvent;
    struct timeval mTime;
};

SBF_END_DECLS

#endif /* _SBF_TIMER_H_ */
