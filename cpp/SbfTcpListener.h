#pragma once

#include "sbfTcpListener.h"


class SbfTcpListenerDelegate {
public:
    virtual ~SbfTcpListenerDelegate () { }

    virtual void onReady () { }

    virtual void onAccept (struct sbfTcpConnection Impl* tc) { }
};

class SbfTcpListener {
public:
    SbfTcpListener (sbfLog log,
                    struct sbfMwThreadImpl* thread,
                    struct sbfQueueImpl* queue,
                    sbfTcpConnectionAddress* address,
                    bool isUnix,
                    SbfTcpListenerDelegate* delegate)
        : mListener (NULL),
          mLog (log),
          mThread(thread),
          mQueue (queue),
          mAddress (address),
          mIsUnix (isUnix),
          mDelegate (delegate)
    {
    }

    virtual ~SbfTcpListener ()
    {
        if (getHandle () != NULL)
            sbfTcpListener_destroy (getHandle ());

    }

    virtual sbfTcpListener getHandle ()
    {
        return mListener;
    }

    virtual bool connect ()
    {
        mListener = sbfTcpListener_create (mLog,
                                           mThread,
                                           mQueue,
                                           mAddress,
                                           mIsUnix ? 1 : 0,
                                           SbfTcpListener::sbfTcpListenerReadyCb,
                                           SbfTcpListener::sbfTcpListenerAcceptCb,
                                           this);
        return mListener !=NULL;
    }

    virtual bool isUnix ()
    {
        return sbfTcpListener_isUnix (getHandle ());
    }

protected:
        sbfTcpListener          mListener;
        sbfLog                  mLog;
        struct sbfMwThreadImpl* mThread;
        struct sbfQueueImpl*    mQueue;
        sbfTcpListenerAddress   mAddress;
        bool                    mIsUnix;
        SbfTcpListenerDelegate* mDelegate;

private:
    static void sbfTcpListenerReadyCb (sbfTcpListener t1, void* closure)
    {
        SbfTcpListener* instance = static_cast<SbfTcpListener*>(closure);
        if (instance->mDelegate)
        {
            instance->mDelegate->onReady ();
        }
    }

    static void sbfTcpListenerAcceptCb (sbfTcpListener t1, struct sbfTcpConnection Impl* tc, void* closure)
    {
        SbfTcpListener* instance = static_cast<SbfTcpListener*>(closure);
        if (instance->mDelegate)
        {
            instance->mDelegate->onAccept ();
        }
    }
};
