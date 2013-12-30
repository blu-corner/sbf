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
    bufferevent_enable (tc->mEvent, EV_READ|EV_WRITE);

    return 0;
}

sbfTcpConnection
sbfTcpConnection_wrap (int s)
{
    sbfTcpConnection tc;

    evutil_make_socket_closeonexec (s);
    evutil_make_socket_nonblocking (s);

    tc = xcalloc (1, sizeof *tc);
    tc->mSocket = s;

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
    sbfTcpConnection tc;
    int              s;

    s = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s < 0)
        return NULL;

    tc = sbfTcpConnection_wrap (s);
    if (tc == NULL)
        return NULL;
    sbfLog_debug ("creating %p", tc);

    if (sbfTcpConnectionSet (tc,
                             thread,
                             queue,
                             readyCb,
                             errorCb,
                             readCb,
                             closure) != 0)
        goto fail;

    if (bufferevent_socket_connect_hostname (tc->mEvent,
                                             NULL, /* DNS can block */
                                             AF_INET,
                                             address,
                                             port) != 0)
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
    sbfLog_debug ("accepting %p", tc);

    return sbfTcpConnectionSet (tc,
                                thread,
                                queue,
                                readyCb,
                                errorCb,
                                readCb,
                                closure);
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
