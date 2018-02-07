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

    if (tc->mIsUnix)
    {
        sbfLog_debug (tc->mLog,
                      "TCP connection %p %s ready",
                      tc,
                      tc->mUnixPeer.sun_path);
    }
    else
    {
        char             tmp[INET_ADDRSTRLEN];
        inet_ntop (AF_INET, &tc->mPeer.sin_addr, tmp, sizeof tmp);
        sbfLog_debug (tc->mLog,
                      "TCP connection %p (%s:%hu) ready",
                      tc,
                      tmp,
                      ntohs (tc->mPeer.sin_port));
    }

    if (tc->mReadyCb != NULL && !tc->mDestroyed)
        tc->mReadyCb (tc, tc->mClosure);

    if (sbfRefCount_decrement (&tc->mRefCount))
        free (tc);
}

static void
sbfTcpConnectionErrorQueueCb (sbfQueueItem item, void* closure)
{
    sbfTcpConnection tc = closure;

    if (tc->mIsUnix)
    {
        sbfLog_debug (tc->mLog,
                      "TCP connection %p %s error",
                      tc,
                      tc->mUnixPeer.sun_path);
    }
    else
    {
        char             tmp[INET_ADDRSTRLEN];

        inet_ntop (AF_INET, &tc->mPeer.sin_addr, tmp, sizeof tmp);
        sbfLog_debug (tc->mLog,
                      "TCP connection %p (%s:%hu) error",
                      tc,
                      tmp,
                      ntohs (tc->mPeer.sin_port));
    }

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
sbfTcpConnection_wrap (sbfLog log, int s, struct sockaddr_in* sin)
{
    sbfTcpConnection tc;

    evutil_make_socket_closeonexec (s);
    evutil_make_socket_nonblocking (s);

    tc = xcalloc (1, sizeof *tc);
    tc->mLog = log;
    tc->mSocket = s;
    tc->mIsUnix = false;
    memcpy (&tc->mPeer, sin, sizeof tc->mPeer);

    return tc;
}

sbfTcpConnection
sbfTcpConnection_wrapUnix (sbfLog log, int s, struct sockaddr_un* sin)
{
    sbfTcpConnection tc;

    evutil_make_socket_closeonexec (s);
    evutil_make_socket_nonblocking (s);

    tc = xcalloc (1, sizeof *tc);
    tc->mLog = log;
    tc->mSocket = s;
    tc->mIsUnix = true;
    memcpy (&tc->mUnixPeer, sin, sizeof tc->mUnixPeer);

    return tc;
}

sbfTcpConnection
sbfTcpConnection_create (sbfLog log,
                         sbfMwThread thread,
                         sbfQueue queue,
                         struct sockaddr_in* sin,
                         sbfTcpConnectionReadyCb readyCb,
                         sbfTcpConnectionErrorCb errorCb,
                         sbfTcpConnectionReadCb readCb,
                         void* closure)
{
    sbfTcpConnection tc;
    int              s;
    char             tmp[INET_ADDRSTRLEN];
    int              error;

    s = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s == -1)
        return NULL;
    tc = sbfTcpConnection_wrap (log, s, sin);
    if (tc == NULL)
        return NULL;

    inet_ntop (AF_INET, &sin->sin_addr, tmp, sizeof tmp);
    sbfLog_debug (log,
                  "creating TCP connection %p to %s:%hu",
                  tc,
                  tmp,
                  ntohs (sin->sin_port));

    error =  sbfTcpConnectionSet (tc,
                                  thread,
                                  queue,
                                  readyCb,
                                  errorCb,
                                  readCb,
                                  closure);
    if (error != 0)
        goto fail;

    error = bufferevent_socket_connect (tc->mEvent,
                                        (struct sockaddr*)sin,
                                        sizeof *sin);
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

sbfTcpConnection
sbfTcpConnection_createUnix (sbfLog log,
                             sbfMwThread thread,
                             sbfQueue queue,
                             struct sockaddr_un* sin,
                             sbfTcpConnectionReadyCb readyCb,
                             sbfTcpConnectionErrorCb errorCb,
                             sbfTcpConnectionReadCb readCb,
                             void* closure)
{
    sbfTcpConnection tc;
    int              s;
    int              error;

    s = socket (AF_UNIX, SOCK_STREAM, IPPROTO_TCP);
    if (s == -1)
        return NULL;
    tc = sbfTcpConnection_wrapUnix (log, s, sin);
    if (tc == NULL)
        return NULL;

    sbfLog_debug (log,
                  "creating UNIX connection %p to %s",
                  tc,
                  sin->sun_path);

    error =  sbfTcpConnectionSet (tc,
                                  thread,
                                  queue,
                                  readyCb,
                                  errorCb,
                                  readCb,
                                  closure);
    if (error != 0)
        goto fail;

    error = bufferevent_socket_connect (tc->mEvent,
                                        (struct sockaddr*)sin,
                                        sizeof *sin);
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
    char     tmp[INET_ADDRSTRLEN];
    sbfError error;

    inet_ntop (AF_INET, &tc->mPeer.sin_addr, tmp, sizeof tmp);
    sbfLog_debug (tc->mLog,
                  "accepting TCP connection %p from %s:%hu",
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

struct sockaddr_in*
sbfTcpConnection_getPeer (sbfTcpConnection tc)
{
    return &tc->mPeer;
}

struct sockaddr_un*
sbfTcpConnection_getPeerUnix (sbfTcpConnection tc)
{
    return &tc->mUnixPeer;
}

bool
sbfTcpConnection_isUnix (sbfTcpConnection tc)
{
    return tc->mIsUnix;
}
