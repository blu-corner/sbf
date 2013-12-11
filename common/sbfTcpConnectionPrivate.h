#ifndef _SBF_TCP_CONNECTION_PRIVATE_H_
#define _SBF_TCP_CONNECTION_PRIVATE_H_

SBF_BEGIN_DECLS

#include "sbfMw.h"
#include "sbfMwInternal.h"
#include "sbfMwPrivate.h"
#include "sbfRefCount.h"

struct sbfTcpListenerImpl;

struct sbfTcpConnectionImpl
{
    sbfSocket                  mSocket;
    struct bufferevent*        mEvent;

    sbfTcpConnectionReadyCb    mReadyCb;
    sbfTcpConnectionReadCb     mReadCb;
    void*                      mClosure;

    sbfMwThread                mThread;
    sbfQueue                   mQueue;

    int                        mDestroyed;
    sbfRefCount                mRefCount;

    struct sbfTcpListenerImpl* mListener;
};

sbfTcpConnection sbfTcpConnection_wrap (int socket);

SBF_END_DECLS

#endif /* _SBF_TCP_CONNECTION_PRIVATE_H_ */
