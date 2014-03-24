#include "sbfTcpConnection.h"
#include "sbfTcpConnectionPrivate.h"

static void
sbfTcpConnectionReadyQueueCb (sbfQueueItem item, void* closure)
{
    sbfTcpConnection tc = closure;

    if (tc->mReadyCb != NULL && !tc->mDestroyed)
        tc->mReadyCb (tc, tc->mClosure);

    if (sbfRefCount_decrement (&tc->mRefCount))
        free (tc);
}

static void
sbfTcpConnectionErrorQueueCb (sbfQueueItem item, void* closure)
{
    sbfTcpConnection tc = closure;

    if (tc->mErrorCb != NULL && !tc->mDestroyed)
        tc->mErrorCb (tc, tc->mClosure);

    if (sbfRefCount_decrement (&tc->mRefCount))
        free (tc);
}

static void
sbfTcpConnectionEventReadCb (struct bufferevent* bev, void* closure)
{
    sbfTcpConnection tc = closure;
    struct evbuffer* evb;
    size_t           used;

    evb = bufferevent_get_input (bev);
    used = tc->mReadCb (tc,
                        evbuffer_pullup (evb, -1),
                        evbuffer_get_length (evb),
                        tc->mClosure);
    evbuffer_drain (evb, used);
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
sbfTcpConnection_wrap (int s, struct sockaddr_in* sin)
{
    sbfTcpConnection tc;

    evutil_make_socket_closeonexec (s);
    evutil_make_socket_nonblocking (s);

    tc = xcalloc (1, sizeof *tc);
    tc->mSocket = s;
    memcpy (&tc->mPeer, sin, sizeof tc->mPeer);

    return tc;
}

sbfTcpConnection
sbfTcpConnection_create (sbfMwThread thread,
                         sbfQueue queue,
                         const char* address,
                         uint16_t port,
                         sbfTcpConnectionReadyCb readyCb,
                         sbfTcpConnectionErrorCb errorCb,
                         sbfTcpConnectionReadCb readCb,
                         void* closure)
{
    sbfTcpConnection   tc;
    int                s;
    struct addrinfo    hints, *res, *res0;
    int                error;
    char               port0[16];
    struct sockaddr_in sin;
    char               tmp[INET_ADDRSTRLEN];

    memset (&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    snprintf (port0, sizeof port0, "%hu", port);
    error = evutil_getaddrinfo (address, port0, &hints, &res0);
    if (error != 0)
        return NULL;
    s = -1;
    for (res = res0; res != NULL; res = res->ai_next) {
        s = socket (res->ai_family, res->ai_socktype, res->ai_protocol);
        if (s != -1)
            break;
    }
    if (s == -1)
        return NULL;
    memcpy (&sin, res->ai_addr, sizeof sin);
    freeaddrinfo (res0);

    tc = sbfTcpConnection_wrap (s, &sin);
    if (tc == NULL)
        return NULL;

    inet_ntop (AF_INET, &sin.sin_addr, tmp, sizeof tmp);
    sbfLog_debug ("creating %p to %s:%hu", tc, tmp, ntohs (sin.sin_port));

    if (sbfTcpConnectionSet (tc,
                             thread,
                             queue,
                             readyCb,
                             errorCb,
                             readCb,
                             closure) != 0)
        goto fail;

    if (bufferevent_socket_connect (tc->mEvent,
                                    (struct sockaddr*)&sin,
                                    sizeof sin) != 0)
        goto fail;

    return tc;

fail:
    sbfTcpConnection_destroy (tc);
    return NULL;
}

void
sbfTcpConnection_destroy (sbfTcpConnection tc)
{
    sbfLog_debug ("destroying %p", tc);

    tc->mDestroyed = 1;
    if (tc->mEvent != NULL)
        bufferevent_free (tc->mEvent);
    EVUTIL_CLOSESOCKET (tc->mSocket);

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
    char     tmp[INET_ADDRSTRLEN];
    sbfError error;

    inet_ntop (AF_INET, &tc->mPeer.sin_addr, tmp, sizeof tmp);
    sbfLog_debug ("accepting %p from %s:%hu",
                  tc,
                  tmp,
                  ntohs (tc->mPeer.sin_port));

    error = sbfTcpConnectionSet (tc,
                                  thread,
                                  queue,
                                  readyCb,
                                  errorCb,
                                  readCb,
                                  closure);
    if (error == 0)
        bufferevent_enable (tc->mEvent, EV_READ|EV_WRITE);
    return error;
}

void
sbfTcpConnection_send (sbfTcpConnection tc, void* data, size_t size)
{
    bufferevent_write (tc->mEvent, data, size);
}

void
sbfTcpConnection_sendBuffer (sbfTcpConnection tc, sbfBuffer buffer)
{
    bufferevent_write (tc->mEvent,
                       sbfBuffer_getData (buffer),
                       sbfBuffer_getSize (buffer));
}

struct sockaddr_in*
sbfTcpConnection_getPeer (sbfTcpConnection tc)
{
    return &tc->mPeer;
}
