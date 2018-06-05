/*!
   \file sbfTcpMeshHandler.h
   \brief This file declares the structures and functions to handle TCP
   connections.
   \Copyright 2014-2018 Neueda Ltd.
*/
#ifndef _SBF_TCP_MESH_HANDLER_H_
#define _SBF_TCP_MESH_HANDLER_H_

#include "sbfHandler.h"
#include "sbfInterface.h"
#include "sbfMw.h"
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

/** \brief Defautl TCP port */
#define SBF_TCP_MESH_HANDLER_DEFAULT_PORT 12222

/** \brief TCP Mesh connection interval */
#define SBF_TCP_MESH_HANDLER_CONNECT_INTERVAL { 3, 0 }

/** \brief TCP mesh connection definition */
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

/** \brief TCP mesh declaration. */
typedef struct sbfTcpMeshHandlerConnectionImpl* sbfTcpMeshHandlerConnection;

/** \brief TCP mesh handler definition. */
struct sbfTcpMeshHandlerImpl
{
    sbfLog                                         mLog;

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

/** \brief TCP mesh handler declaration. */
typedef struct sbfTcpMeshHandlerImpl* sbfTcpMeshHandler;

SBF_END_DECLS

#endif /* _SBF_TCP_MESH_HANDLER_H_ */
