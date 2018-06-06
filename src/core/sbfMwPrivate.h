#ifndef _SBF_MW_PRIVATE_H_
#define _SBF_MW_PRIVATE_H_

#include <event.h>
#include <event2/thread.h>
#include <event2/listener.h>

SBF_BEGIN_DECLS

struct sbfMwImpl
{
    sbfLog      mLog;
    sbfKeyValue mProperties;
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
    sbfThread          mThread;
};

void sbfMw_enqueueThread (sbfMwThread thread,
                          struct event* event,
                          event_callback_fn cb,
                          void* closure);

SBF_END_DECLS

#endif /* _SBF_MW_PRIVATE_H_ */
