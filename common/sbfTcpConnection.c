#include "sbfTcpConnection.h"
#include "sbfTcpConnectionPrivate.h"

static void
sbfTcpConnectionEventReadCb (struct bufferevent* be, void* closure)
{
    sbfTcpConnection tc = closure;

    /* XXX */
}

static void
sbfTcpConnectionEventEventCb (struct bufferevent* be,
                              short events,
                              void* closure)
{
    sbfTcpConnection tc = closure;

    /* XXX */
}

static sbfError
sbfTcpConnectionSet (sbfTcpConnection tc,
                     sbfMwThread thread,
                     sbfQueue queue,
                     sbfTcpConnectionReadyCb readyCb,
                     sbfTcpConnectionReadCb readCb,
                     void* closure)
{
    tc->mThread = thread;
    tc->mQueue = queue;

    tc->mReadyCb = readyCb;
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
sbfTcpConnection_wrap (int s)
{
    sbfTcpConnection tc;

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

    if (sbfTcpConnectionSet (tc, thread, queue, readyCb, readCb, closure) != 0)
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
    close (tc->mSocket);

    if (sbfRefCount_decrement (&tc->mRefCount))
        free (tc);
}

sbfError
sbfTcpConnection_accept (sbfTcpConnection tc,
                         sbfMwThread thread,
                         sbfQueue queue,
                         sbfTcpConnectionReadyCb readyCb,
                         sbfTcpConnectionReadCb readCb,
                         void* closure)
{
    sbfLog_debug ("accepting %p", tc);

    return sbfTcpConnectionSet (tc, thread, queue, readyCb, readCb, closure);
}

void
sbfTcpConnection_send (sbfTcpConnection tc, void* data, size_t size)
{
    /* XXX */
}

void
sbfTcpConnection_sendBuffer(sbfTcpConnection tc, sbfBuffer buffer)
{
    /* XXX */
}
