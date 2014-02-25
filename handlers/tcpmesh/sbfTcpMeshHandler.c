#include "sbfTcpMeshHandler.h"

static void sbfTcpMeshHandlerFreeConnection (sbfTcpMeshHandlerConnection c);
static void sbfTcpMeshHandlerSetSocket (sbfTcpMeshHandlerConnection c,
                                        evutil_socket_t s);
static void sbfTcpMeshHandlerCloseSocket (sbfTcpMeshHandlerConnection c);

static void
sbfTcpMeshHandlerConnectionTimerCb (int fd, short events, void* closure)
{
    sbfTcpMeshHandlerConnection c = closure;
    int                         error;
    struct timeval              tv = SBF_TCP_MESH_HANDLER_CONNECT_INTERVAL;
    evutil_socket_t             s;

    sbfLog_info ("trying connection %p to %s:%hu", c, c->mHost, c->mPort);

    sbfTcpMeshHandlerCloseSocket (c);

    s = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s < 0)
        goto fail;
    sbfTcpMeshHandlerSetSocket (c, s);

    error = bufferevent_socket_connect_hostname (c->mEvent,
                                                 NULL,
                                                 AF_INET,
                                                 c->mHost,
                                                 c->mPort);
    if (error != 0)
        goto fail;

    return;

fail:
    sbfLog_info ("connection %p failed", c);
    event_add (&c->mTimer, &tv);
}

static void
sbfTcpMeshHandlerConnectionReadCb (struct bufferevent* bev, void* closure)
{
    sbfTcpMeshHandlerConnection c = closure;
    struct evbuffer*            evb = bufferevent_get_input (bev);
    void*                       data = evbuffer_pullup (evb, -1);
    size_t                      size;
    sbfBuffer                   buffer;

    size = sbfHandler_size (data, evbuffer_get_length (evb));
    if (size == 0)
        return;

    if (c->mParent->mHandle != NULL)
    {
        buffer = sbfBuffer_new (c->mParent->mPool, size);
        memcpy (sbfBuffer_getData (buffer), data, size);

        sbfHandler_message (c->mParent->mHandle, buffer);
        sbfBuffer_destroy (buffer);
    }
    evbuffer_drain (evb, size);
}

static void
sbfTcpMeshHandlerConnectionEventCb (struct bufferevent* bev,
                                    short events,
                                    void* closure)
{
    sbfTcpMeshHandlerConnection c = closure;
    struct timeval              tv = SBF_TCP_MESH_HANDLER_CONNECT_INTERVAL;

    if (events & BEV_EVENT_CONNECTED)
    {
        c->mConnected = 1;
        bufferevent_enable (c->mEvent, EV_READ|EV_WRITE);
        sbfLog_info ("connection %p connected", c);
    }

    if (events & (BEV_EVENT_ERROR|BEV_EVENT_TIMEOUT|BEV_EVENT_EOF))
    {
        c->mConnected = 0;
        bufferevent_disable (c->mEvent, EV_READ|EV_WRITE);
        sbfLog_info ("connection %p failed", c);

        if (c->mIncoming)
            sbfTcpMeshHandlerFreeConnection (c);
        else
            event_add (&c->mTimer, &tv);
    }
}

static sbfTcpMeshHandlerConnection
sbfTcpMeshHandlerNewConnection (sbfTcpMeshHandler tmh, int incoming)
{
    sbfTcpMeshHandlerConnection c;

    c = xcalloc (1, sizeof *c);
    c->mParent = tmh;

    sbfMutex_lock (&tmh->mMutex);
    TAILQ_INSERT_TAIL (&tmh->mConnections, c, mEntry);
    sbfMutex_unlock (&tmh->mMutex);

    event_assign (&c->mTimer,
                  tmh->mEventBase,
                  -1,
                  0,
                  sbfTcpMeshHandlerConnectionTimerCb,
                  c);
    c->mIncoming = incoming;

    c->mSocket = -1;
    c->mEvent = NULL;

    return c;
}

static void
sbfTcpMeshHandlerSetSocket (sbfTcpMeshHandlerConnection c, evutil_socket_t s)
{
    sbfTcpMeshHandler tmh = c->mParent;

    evutil_make_socket_closeonexec (s);
    evutil_make_socket_nonblocking (s);

    c->mEvent = bufferevent_socket_new (tmh->mEventBase, s, BEV_OPT_THREADSAFE);
    if (c->mEvent == NULL)
        sbfFatal ("couldn't allocate event");
    c->mSocket = s;

    bufferevent_setcb (c->mEvent,
                       sbfTcpMeshHandlerConnectionReadCb,
                       NULL,
                       sbfTcpMeshHandlerConnectionEventCb,
                       c);
}

static void
sbfTcpMeshHandlerCloseSocket (sbfTcpMeshHandlerConnection c)
{
    if (c->mEvent != NULL)
        bufferevent_free (c->mEvent);
    c->mEvent = NULL;

    if (c->mSocket != -1)
        EVUTIL_CLOSESOCKET (c->mSocket);
    c->mSocket = -1;
}

static void
sbfTcpMeshHandlerFreeConnection (sbfTcpMeshHandlerConnection c)
{
    sbfTcpMeshHandler tmh = c->mParent;

    event_del (&c->mTimer);

    sbfTcpMeshHandlerCloseSocket (c);

    free ((void*)c->mHost);

    sbfMutex_lock (&tmh->mMutex);
    TAILQ_REMOVE (&tmh->mConnections, c, mEntry);
    sbfMutex_unlock (&tmh->mMutex);

    free (c);
}

static void
sbfTcpMeshHandlerListenerAcceptCb (struct evconnlistener* listener,
                                   evutil_socket_t s,
                                   struct sockaddr* address,
                                   int length,
                                   void* closure)
{
    sbfTcpMeshHandler           tmh = closure;
    sbfTcpMeshHandlerConnection c;
    char                        tmp[INET_ADDRSTRLEN];
    struct sockaddr_in*         sin = (struct sockaddr_in*)address;

    c = sbfTcpMeshHandlerNewConnection (tmh, 1);
    sbfTcpMeshHandlerSetSocket (c, s);

    c->mConnected = 1;
    bufferevent_enable (c->mEvent, EV_READ|EV_WRITE);

    inet_ntop (AF_INET, &sin->sin_addr, tmp, sizeof tmp);
    sbfLog_info ("incoming connection %p from %s:%hu",
                 c,
                 tmp,
                 ntohs (sin->sin_port));
}

static void
sbfTcpMeshHandlerAddCompleteEventCb (int fd, short events, void* closure)
{
    sbfTcpMeshHandler tmh = closure;

    tmh->mAddStreamCompleteCb (tmh->mHandle, 0, tmh->mClosure);
}

static void
sbfTcpMeshHandlerDestroy (sbfHandler handler)
{
    sbfTcpMeshHandler           tmh = handler;
    sbfTcpMeshHandlerConnection c;
    sbfTcpMeshHandlerConnection c1;

    TAILQ_FOREACH_SAFE (c, &tmh->mConnections, mEntry, c1)
        sbfTcpMeshHandlerFreeConnection (c);
    sbfMutex_destroy (&tmh->mMutex);

    if (tmh->mListener != NULL)
        evconnlistener_free (tmh->mListener);

    sbfPool_destroy (tmh->mPool);
    free (tmh);
}

static sbfHandler
sbfTcpMeshHandlerCreate (sbfTport tport, sbfKeyValue properties)
{
    sbfTcpMeshHandler           tmh;
    struct sockaddr_in          sin;
    u_int                       i;
    const char*                 host;
    const char*                 value;
    char*                       endptr;
    sbfTcpMeshHandlerConnection c;
    unsigned long               port;

    tmh = xcalloc (1, sizeof *tmh);
    sbfMutex_init (&tmh->mMutex, 0);
    TAILQ_INIT (&tmh->mConnections);
    tmh->mPool = sbfPool_create (65536);

    tmh->mHandle = NULL;
    tmh->mThread = sbfMw_getDefaultThread (sbfTport_getMw (tport));
    tmh->mEventBase = sbfMw_getThreadEventBase (tmh->mThread);

    port = SBF_TCP_MESH_HANDLER_DEFAULT_PORT;
    if ((value = sbfKeyValue_get (properties, "listen")) != NULL)
    {
        port = strtoul (value, &endptr, 10);
        if (port > 65536 || *endptr != '\0')
        {
            sbfLog_err ("invalid port in %s", value);
            goto fail;
        }
    }
    if (port != 0)
    {
        memset (&sin, 0, sizeof sin);
        sin.sin_family = AF_INET;
        sin.sin_addr.s_addr = INADDR_ANY;
        sin.sin_port = htons (port);
        tmh->mListener = evconnlistener_new_bind (
                                              tmh->mEventBase,
                                              sbfTcpMeshHandlerListenerAcceptCb,
                                              tmh,
                                              LEV_OPT_THREADSAFE|
                                              LEV_OPT_CLOSE_ON_FREE|
                                              LEV_OPT_CLOSE_ON_EXEC|
                                              LEV_OPT_REUSEABLE,
                                              -1,
                                              (struct sockaddr*)&sin,
                                              sizeof sin);
        if (tmh->mListener == NULL)
        {
            sbfLog_err ("failed to create listener");
            goto fail;
        }
    }

    for (i = 0; i < USHRT_MAX; i++)
    {
        host = sbfKeyValue_getV (properties, "connect%u", i);
        if (host == NULL)
            continue;

        c = sbfTcpMeshHandlerNewConnection (tmh, 0);
        if (c == NULL)
            goto fail;

        c->mHost = xstrdup (host);
        if ((endptr = strchr (c->mHost, ':')) != NULL)
        {
            *endptr++ = '\0';

            c->mPort = strtoul (endptr, &endptr, 10);
            if (port == 0 || port > 65536 || *endptr != '\0')
            {
                sbfLog_err ("invalid port in %s", host);
                goto fail;
            }
        }
        else
            c->mPort = SBF_TCP_MESH_HANDLER_DEFAULT_PORT;

        sbfLog_info ("outgoing connection %p to %s:%hu", c, c->mHost, c->mPort);
        event_active (&c->mTimer, EV_TIMEOUT, 0);
    }

    return tmh;

fail:
    sbfTcpMeshHandlerDestroy (tmh);
    return NULL;
}

sbfHandlerHandle
sbfTcpMeshHandlerFindStream (sbfHandler handler, sbfTopic topic)
{
    sbfTcpMeshHandler tmh = handler;

    return tmh->mHandle;
}

sbfHandlerStream
sbfTcpMeshHandlerAddStream (sbfHandler handler,
                            sbfTopic topic,
                            sbfMwThread thread,
                            sbfHandlerHandle handle,
                            sbfHandlerAddStreamCompleteCb cb,
                            void* closure)
{
    sbfTcpMeshHandler tmh = handler;

    SBF_ASSERT (thread == tmh->mThread); /* must be first thread */

    SBF_ASSERT (tmh->mHandle == NULL); /* must be only one handle */
    tmh->mHandle = handle;

    tmh->mAddStreamCompleteCb = cb;
    tmh->mClosure = closure;

    if (cb != NULL)
    {
        sbfMw_enqueue (tmh->mThread,
                       &tmh->mEventAdd,
                       sbfTcpMeshHandlerAddCompleteEventCb,
                       tmh);
    }

    return tmh;
}

void
sbfTcpMeshHandlerRemoveStream (sbfHandlerStream stream)
{
}

static sbfBuffer
sbfTcpMeshHandlerGetBuffer (sbfHandlerStream stream, size_t size)
{
    return sbfBuffer_new (NULL, size);
}

static void
sbfTcpMeshHandlerSendBuffer (sbfHandlerStream stream, sbfBuffer buffer)
{
    sbfTcpMeshHandler           tmh = stream;
    sbfTcpMeshHandlerConnection c;
    void*                       data = sbfBuffer_getData (buffer);
    size_t                      size = sbfBuffer_getSize (buffer);

    sbfMutex_lock (&tmh->mMutex);
    TAILQ_FOREACH (c, &tmh->mConnections, mEntry)
    {
        if (c->mConnected)
            evbuffer_add (bufferevent_get_output (c->mEvent), data, size);
    }
    sbfMutex_unlock (&tmh->mMutex);
}

SBF_DLLEXPORT sbfHandlerTable sbf_tcpmesh_handler =
{
    sbfTcpMeshHandlerCreate,
    sbfTcpMeshHandlerDestroy,
    sbfTcpMeshHandlerFindStream,
    sbfTcpMeshHandlerAddStream,
    sbfTcpMeshHandlerRemoveStream,
    sbfTcpMeshHandlerGetBuffer,
    sbfTcpMeshHandlerSendBuffer
};
