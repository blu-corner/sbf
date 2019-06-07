#include "sbfTcpConnection.h"
#include "sbfTcpConnectionPrivate.h"

static void
sbfTcpConnectionBufferFreeCb (const void *data, size_t datalen, void* closure)
{
    sbfBuffer_destroy (closure);
}

static void
sbfTcpConnectionReadyQueueCb (sbfQueueItem item, void* closure)
{
    sbfTcpConnection tc = closure;

#ifndef WIN32
        sbfLog_debug (tc->mLog,
                      "TCP connection %p %s ready",
                      tc,
                      tc->mPeer.sun.sun_path);
#else
        char             tmp[INET_ADDRSTRLEN];
        inet_ntop (AF_INET, &tc->mPeer.sin.sin_addr, tmp, sizeof tmp);
        sbfLog_debug (tc->mLog,
                      "TCP connection %p (%s:%hu) ready",
                      tc,
                      tmp,
                      ntohs (tc->mPeer.sin.sin_port));
#endif

    if (tc->mReadyCb != NULL && !tc->mDestroyed)
        tc->mReadyCb (tc, tc->mClosure);

    if (sbfRefCount_decrement (&tc->mRefCount))
        free (tc);
}

static void
sbfTcpConnectionErrorQueueCb (sbfQueueItem item, void* closure)
{
    sbfTcpConnection tc = closure;

#ifndef WIN32
        sbfLog_debug (tc->mLog,
                      "TCP connection %p %s error",
                      tc,
                      tc->mPeer.sun.sun_path);
#else
        char             tmp[INET_ADDRSTRLEN];

        inet_ntop (AF_INET, &tc->mPeer.sin.sin_addr, tmp, sizeof tmp);
        sbfLog_debug (tc->mLog,
                      "TCP connection %p (%s:%hu) error",
                      tc,
                      tmp,
                      ntohs (tc->mPeer.sin.sin_port));
#endif

    if (tc->mErrorCb != NULL && !tc->mDestroyed)
        tc->mErrorCb (tc, tc->mClosure);

    if (sbfRefCount_decrement (&tc->mRefCount))
        free (tc);
}

static void
sbfTcpConnectionReadQueueCb (sbfQueueItem item, void* closure)
{
    sbfTcpConnection tc = closure;
    struct evbuffer* evb = bufferevent_get_input (tc->mEvent);
    size_t           size;
    size_t           used;

    if (!tc->mDestroyed)
    {
        size = evbuffer_get_length (evb);
        if (size != 0)
        {
            used = tc->mReadCb (tc,
                                evbuffer_pullup (evb, -1),
                                size,
                                tc->mClosure);
            if (!tc->mDestroyed)
                evbuffer_drain (evb, used);
        }

        if (!tc->mDestroyed)
        {
            tc->mQueued = 0;
            bufferevent_enable (tc->mEvent, EV_READ);
        }
    }

    if (sbfRefCount_decrement (&tc->mRefCount))
        free (tc);
}

static void
sbfTcpConnectionEventReadCb (struct bufferevent* bev, void* closure)
{
    sbfTcpConnection tc = closure;

    if (tc->mQueued)
        return;

    tc->mQueued = 1;
    bufferevent_disable (tc->mEvent, EV_READ);

    sbfRefCount_increment (&tc->mRefCount);
    sbfQueue_enqueue (tc->mQueue, sbfTcpConnectionReadQueueCb, tc);
}

static void
sbfTcpConnectionEventEventCb (struct bufferevent* bev,
                              short events,
                              void* closure)
{
    sbfTcpConnection tc = closure;

    if (events & BEV_EVENT_CONNECTED)
    {
        bufferevent_enable (tc->mEvent, EV_READ|EV_WRITE);
        sbfRefCount_increment (&tc->mRefCount);
        sbfQueue_enqueue (tc->mQueue, sbfTcpConnectionReadyQueueCb, tc);
        return;
    }

    if (events & (BEV_EVENT_ERROR|BEV_EVENT_TIMEOUT|BEV_EVENT_EOF))
    {
        if (tc->mEvent != NULL)
            bufferevent_free (tc->mEvent);
        tc->mEvent = NULL;

        sbfRefCount_increment (&tc->mRefCount);
        sbfQueue_enqueue (tc->mQueue, sbfTcpConnectionErrorQueueCb, tc);
        return;
    }
}

static sbfError
sbfTcpConnectionSet (sbfTcpConnection tc,
                     sbfMwThread thread,
                     sbfQueue queue,
                     sbfTcpConnectionReadyCb readyCb,
                     sbfTcpConnectionErrorCb errorCb,
                     sbfTcpConnectionReadCb readCb,
                     void* closure)
{
    tc->mThread = thread;
    tc->mQueue = queue;

    tc->mReadyCb = readyCb;
    tc->mErrorCb = errorCb;
    tc->mReadCb = readCb;
    tc->mClosure = closure;

    tc->mDestroyed = 0;
    sbfRefCount_init (&tc->mRefCount, 1);

    tc->mQueued = 0;
    tc->mEvent = bufferevent_socket_new (sbfMw_getThreadEventBase (thread),
                                         tc->mSocket,
                                         BEV_OPT_THREADSAFE);
    if (tc->mEvent == NULL)
        return EIO;
    bufferevent_setcb (tc->mEvent,
                       sbfTcpConnectionEventReadCb,
                       NULL,
                       sbfTcpConnectionEventEventCb,
                       tc);

    return 0;
}

sbfTcpConnection
sbfTcpConnection_wrap (sbfLog log,
                       int socket,
                       int isUnix,
                       int disableNagles,
                       sbfTcpConnectionAddress* address)
{
    sbfTcpConnection tc;

    evutil_make_socket_closeonexec (socket);
    evutil_make_socket_nonblocking (socket);

    if (disableNagles)
    {
        int flag = 1;
        int result = setsockopt (socket,
                                 IPPROTO_TCP,
                                 TCP_NODELAY,
                                 (char *)&flag,
                                 sizeof (int));
        if (result < 0)
            sbfLog_err (log, "failed to disabled nagles on tcp-socket");
    }

    tc = xcalloc (1, sizeof *tc);
    tc->mLog = log;
    tc->mSocket = socket;
    tc->mIsUnix = isUnix;
    memcpy (&tc->mPeer, address, sizeof(sbfTcpConnectionAddress));

    return tc;
}

sbfTcpConnection
sbfTcpConnection_create (sbfLog log,
                         sbfMwThread thread,
                         sbfQueue queue,
                         sbfTcpConnectionAddress* address,
                         int isUnix,
                         int disableNagles,
                         sbfTcpConnectionReadyCb readyCb,
                         sbfTcpConnectionErrorCb errorCb,
                         sbfTcpConnectionReadCb readCb,
                         void* closure)
{
    sbfTcpConnection tc;
    int              s;
    char             tmp[INET_ADDRSTRLEN];
    int              error;

#if WIN32
    // unix sockets are unsupported on windows
    if (isUnix)
    {
        sbfLog_err (log, "unix-sockets not supported on windows");
        return NULL;
    }
#endif
    
    s = socket (isUnix == 0 ? AF_INET : AF_UNIX,
                SOCK_STREAM,
                IPPROTO_TCP);
    if (s == -1)
        return NULL;
    tc = sbfTcpConnection_wrap (log, s, isUnix, disableNagles, address);
    if (tc == NULL)
        return NULL;

    if (isUnix == 0)
    {
        inet_ntop (AF_INET, &(address->sin.sin_addr), tmp, sizeof tmp);
        sbfLog_debug (log,
                      "creating TCP connection %p to %s:%hu",
                      tc,
                      tmp,
                      ntohs (address->sin.sin_port));
    }

    error = sbfTcpConnectionSet (tc,
                                 thread,
                                 queue,
                                 readyCb,
                                 errorCb,
                                 readCb,
                                 closure);
    if (error != 0)
        goto fail;

    if (isUnix) {
#ifdef WIN32
        goto fail;
#else
        error = bufferevent_socket_connect (tc->mEvent,
                                            (struct sockaddr*)&(address->sun),
                                            sizeof(address->sun));
#endif
    } else {
        error = bufferevent_socket_connect (tc->mEvent,
                                            (struct sockaddr*)&(address->sin),
                                            sizeof(address->sin));
    }

    if (error != 0)
        goto fail;

    return tc;

fail:
    sbfLog_err (tc->mLog,
                "error creating TCP connection %p: %s",
                tc,
                strerror (error));
    sbfTcpConnection_destroy (tc);
    return NULL;
}

void
sbfTcpConnection_destroy (sbfTcpConnection tc)
{
    sbfLog_debug (tc->mLog, "destroying TCP connection %p", tc);

    tc->mDestroyed = 1;
    EVUTIL_CLOSESOCKET (tc->mSocket);
    if (tc->mEvent != NULL)
        bufferevent_free (tc->mEvent);

    if (sbfRefCount_decrement (&tc->mRefCount))
        free (tc);
}

sbfError
sbfTcpConnection_accept (sbfTcpConnection tc,
                         sbfMwThread thread,
                         sbfQueue queue,
                         sbfTcpConnectionReadyCb readyCb,
                         sbfTcpConnectionErrorCb errorCb,
                         sbfTcpConnectionReadCb readCb,
                         void* closure)
{
    sbfError error;

#ifndef WIN32
        sbfLog_debug (tc->mLog,
                      "accepting TCP connection %p %s ready",
                      tc,
                      tc->mPeer.sun.sun_path);
#else
        char     tmp[INET_ADDRSTRLEN];
        inet_ntop (AF_INET, &tc->mPeer.sin.sin_addr, tmp, sizeof tmp);
        sbfLog_debug (tc->mLog,
                      "accepting TCP connection %p from %s:%hu",
                      tc,
                      tmp,
                      ntohs (tc->mPeer.sin.sin_port));
#endif
    
    error = sbfTcpConnectionSet (tc,
                                  thread,
                                  queue,
                                  readyCb,
                                  errorCb,
                                  readCb,
                                  closure);
    if (error == 0)
        bufferevent_enable (tc->mEvent, EV_READ|EV_WRITE);
    else
    {
        sbfLog_err (tc->mLog,
                    "error accepting TCP connection %p: %s",
                    tc,
                    strerror (error));
    }
    return error;
}

void
sbfTcpConnection_send (sbfTcpConnection tc, const void* data, size_t size)
{
    bufferevent_write (tc->mEvent, data, size);
}

void
sbfTcpConnection_sendBuffer (sbfTcpConnection tc, sbfBuffer buffer)
{
    sbfBuffer_addRef (buffer);
    if (evbuffer_add_reference (bufferevent_get_output (tc->mEvent),
                                sbfBuffer_getData (buffer),
                                sbfBuffer_getSize (buffer),
                                sbfTcpConnectionBufferFreeCb,
                                buffer) != 0)
        sbfBuffer_destroy (buffer);
}

sbfTcpConnectionAddress*
sbfTcpConnection_getPeer (sbfTcpConnection tc)
{
    return &tc->mPeer;
}

int
sbfTcpConnection_isUnix (sbfTcpConnection tc)
{
    return tc->mIsUnix;
}
