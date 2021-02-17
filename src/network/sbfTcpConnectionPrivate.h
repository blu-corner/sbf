#ifndef _SBF_TCP_CONNECTION_PRIVATE_H_
#define _SBF_TCP_CONNECTION_PRIVATE_H_

SBF_BEGIN_DECLS

#include "sbfMw.h"
#include "sbfMwPrivate.h"
#include "sbfRefCount.h"

struct sbfTcpListenerImpl;

struct sbfTcpConnectionImpl
{
    sbfLog                     mLog;

    sbfSocket                  mSocket;
    struct bufferevent*        mEvent;

    int                        mIsUnix;
    sbfTcpConnectionAddress    mPeer;

    sbfTcpConnectionReadyCb    mReadyCb;
    sbfTcpConnectionReadCb     mReadCb;
    sbfTcpConnectionErrorCb    mErrorCb;
    void*                      mClosure;

    sbfMwThread                mThread;
    sbfQueue                   mQueue;

    sbfRefCount                mRefCount;

    struct sbfTcpListenerImpl* mListener;
};

sbfTcpConnection sbfTcpConnection_wrap (sbfLog log,
                                        int socket,
                                        int isUnix,
                                        int disableNagles,
                                        sbfTcpConnectionAddress* address);

SBF_END_DECLS

#endif /* _SBF_TCP_CONNECTION_PRIVATE_H_ */
