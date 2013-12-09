#ifndef _SBF_MW_PRIVATE_H_
#define _SBF_MW_PRIVATE_H_

#include "sbfMw.h"
#include "sbfMwInternal.h"

SBF_BEGIN_DECLS

struct sbfMwImpl
{
    int         mStop;

    u_int       mNumThreads;
    sbfMwThread mThreads;
};

struct sbfMwThreadImpl
{
    sbfMw              mParent;
    u_int              mIndex;

    struct event_base* mEventBase;
    struct event       mTimer;
    pthread_t          mThread;
};

SBF_END_DECLS

#endif /* _SBF_MW_PRIVATE_H_ */
