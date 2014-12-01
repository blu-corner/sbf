#ifndef _SBF_TCP_LISTENER_PRIVATE_H_
#define _SBF_TCP_LISTENER_PRIVATE_H_

SBF_BEGIN_DECLS

#include "sbfMw.h"
#include "sbfMwPrivate.h"
#include "sbfRefCount.h"

struct sbfTcpListenerImpl
{
    sbfLog                 mLog;

    struct evconnlistener* mListener;

    sbfTcpListenerReadyCb  mReadyCb;
    sbfTcpListenerAcceptCb mAcceptCb;
    void*                  mClosure;

    sbfMwThread            mThread;
    sbfQueue               mQueue;

    int                    mDestroyed;
    sbfRefCount            mRefCount;
};

SBF_END_DECLS

#endif /* _SBF_TCP_LISTENER_PRIVATE_H_ */
