#include "sbfTcpConnection.h"
#include "sbfTcpConnectionPrivate.h"
#include "sbfTcpListener.h"
#include "sbfTcpListenerPrivate.h"


static void
sbfTcpListenerReadyQueueCb (sbfQueueItem item, void* closure)
{
    sbfTcpListener tl = closure;

    if (tl->mReadyCb != NULL && !tl->mDestroyed)
        tl->mReadyCb (tl, tl->mClosure);

    if (sbfRefCount_decrement (&tl->mRefCount))
        free (tl);
}

static void
sbfTcpListenerAcceptQueueCb (sbfQueueItem item, void* closure)
{
    sbfTcpConnection tc = closure;
    sbfTcpListener   tl = tc->mListener;

    if (!tl->mDestroyed)
        tl->mAcceptCb (tl, tc, tl->mClosure);
    else
        free (tc); /* _wrap doesn't init refcount, so just free here */

    if (sbfRefCount_decrement (&tl->mRefCount))
        free (tl);
}

static void
sbfTcpListenerEventCb (struct evconnlistener* listener,
                       evutil_socket_t s,
                       struct sockaddr* address,
                       int length,
                       void* closure)
{
    sbfTcpListener      tl = closure;
    sbfTcpConnection    tc;
    sbfTcpConnectionAddress sbfAddress;

#ifndef WIN32
	struct sockaddr_un* sun = (struct sockaddr_un*)address;
	memcpy(&(sbfAddress.sun), sun, sizeof(struct sockaddr_un));
#else
	struct sockaddr_in* sin = (struct sockaddr_in*)address;
	memcpy(&(sbfAddress.sin), sin, sizeof(struct sockaddr_in));
#endif

    tc = sbfTcpConnection_wrap (tl->mLog, s, sbfTcpListener_isUnix(tl), 0, &sbfAddress);
    tc->mListener = tl;

    /*
     * There is no need to add a reference to the connection since nobody else
     * has a pointer to it yet.
     */
    sbfRefCount_increment (&tl->mRefCount);
    sbfQueue_enqueue (tl->mQueue, sbfTcpListenerAcceptQueueCb, tc);
}

sbfTcpListener
sbfTcpListener_create (sbfLog log,
                       struct sbfMwThreadImpl* thread,
                       struct sbfQueueImpl* queue,
                       sbfTcpConnectionAddress* address,
                       int isUnix,
                       sbfTcpListenerReadyCb readyCb,
                       sbfTcpListenerAcceptCb acceptCb,
                       void* closure)
{
    sbfTcpListener     tl;

    tl = xcalloc (1, sizeof *tl);
    tl->mLog = log;
    tl->mThread = thread;
    tl->mQueue = queue;

    tl->mReadyCb = readyCb;
    tl->mAcceptCb = acceptCb;
    tl->mClosure = closure;
    tl->mIsUnix = isUnix;

    tl->mDestroyed = 0;
    sbfRefCount_init (&tl->mRefCount, 1);

    if (tl->mIsUnix) {
#ifdef WIN32
        sbfLog_err (tl->mLog, "failed to create event listener unix socket not supported on windows");
        goto fail;
#else
        sbfLog_debug (tl->mLog,
                      "creating TCP listener %p: unix-path %s",
                      tl,
                      address->sun.sun_path);
        tl->mListener = evconnlistener_new_bind (sbfMw_getThreadEventBase (thread),
                                                 sbfTcpListenerEventCb,
                                                 tl,
                                                 LEV_OPT_THREADSAFE|
                                                 LEV_OPT_CLOSE_ON_FREE|
                                                 LEV_OPT_CLOSE_ON_EXEC|
                                                 LEV_OPT_REUSEABLE,
                                                 -1,
                                                 (struct sockaddr*)&(address->sun),
                                                 sizeof(address->sun));
#endif
    } else {
        sbfLog_debug (tl->mLog,
                      "creating TCP listener %p: port %hu",
                      tl,
                      ntohs (address->sin.sin_port));
        tl->mListener = evconnlistener_new_bind (sbfMw_getThreadEventBase (thread),
                                                 sbfTcpListenerEventCb,
                                                 tl,
                                                 LEV_OPT_THREADSAFE|
                                                 LEV_OPT_CLOSE_ON_FREE|
                                                 LEV_OPT_CLOSE_ON_EXEC|
                                                 LEV_OPT_REUSEABLE,
                                                 -1,
                                                 (struct sockaddr*)&(address->sin),
                                                 sizeof(address->sin));
    }
    
    if (tl->mListener == NULL)
    {
        sbfLog_err (tl->mLog, "failed to create event listener");
        goto fail;
    }

    sbfRefCount_increment (&tl->mRefCount);
    sbfQueue_enqueue (queue, sbfTcpListenerReadyQueueCb, tl);

    return tl;

fail:
    sbfTcpListener_destroy (tl);
    return NULL;
}

void
sbfTcpListener_destroy (sbfTcpListener tl)
{
    sbfLog_debug (tl->mLog, "destroying TCP listener %p", tl);

    tl->mDestroyed = 1;
    if (tl->mListener != NULL)
        evconnlistener_free (tl->mListener);

    if (sbfRefCount_decrement (&tl->mRefCount))
        free (tl);
}

int
sbfTcpListener_isUnix (sbfTcpListener tl)
{
    return tl->mIsUnix;
}
