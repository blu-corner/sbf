#ifndef _SBF_TCP_MESH_HANDLER_H_
#define _SBF_TCP_MESH_HANDLER_H_

#include "sbfHandlerInternal.h"
#include "sbfInterface.h"
#include "sbfMw.h"
#include "sbfMwInternal.h"
#include "sbfTport.h"

SBF_BEGIN_DECLS

/*
 * TCP mesh handler. These may be specified in the properties:
 *
 * interface - interface name or address
 *
 * listen - port to listen on
 *
 * connect0,1,2... - ports to connect to
 *
 * Only one thread is supported - all topics go to one stream which should be
 * the first thread.
 */

struct sbfTcpMeshHandlerImpl;

#define SBF_TCP_MESH_HANDLER_DEFAULT_PORT 12222

#define SBF_TCP_MESH_HANDLER_CONNECT_INTERVAL { 3, 0 }

struct sbfTcpMeshHandlerConnectionImpl
{
    struct sbfTcpMeshHandlerImpl*                 mParent;

    struct event                                  mTimer;
    int                                           mIncoming;

    const char*                                   mHost;
    uint16_t                                      mPort;

    sbfSocket                                     mSocket;
    struct bufferevent*                           mEvent;
    int                                           mConnected;

    TAILQ_ENTRY (sbfTcpMeshHandlerConnectionImpl) mEntry;
};
typedef struct sbfTcpMeshHandlerConnectionImpl* sbfTcpMeshHandlerConnection;

struct sbfTcpMeshHandlerImpl
{
    sbfHandlerHandle                               mHandle;
    sbfMwThread                                    mThread;
    struct event_base*                             mEventBase;

    struct event                                   mEventAdd;
    sbfHandlerAddStreamCompleteCb                  mAddStreamCompleteCb;
    void*                                          mClosure;

    struct evconnlistener*                         mListener;
    sbfMutex                                       mMutex;
    TAILQ_HEAD (, sbfTcpMeshHandlerConnectionImpl) mConnections;
    sbfPool                                        mPool;
};
typedef struct sbfTcpMeshHandlerImpl* sbfTcpMeshHandler;

SBF_END_DECLS

#endif /* _SBF_TCP_MESH_HANDLER_H_ */
