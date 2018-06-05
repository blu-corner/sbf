#ifndef _SBF_EVENT_PRIVATE_H_
#define _SBF_EVENT_PRIVATE_H_

#include "sbfMw.h"
#include "sbfMwPrivate.h"
#include "sbfRefCount.h"

SBF_BEGIN_DECLS

struct sbfEventImpl
{
    sbfEventCb   mCb;
    void*        mClosure;

    sbfMwThread  mThread;
    sbfQueue     mQueue;

    int          mDestroyed;
    sbfRefCount  mRefCount;

    struct event mEvent;
};

SBF_END_DECLS

#endif /* _SBF_EVENT_H_ */
