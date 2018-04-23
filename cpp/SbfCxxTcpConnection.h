#pragma once

#include "sbfTcpConnection.h"

namespace neueda
{
class SbfTcpConnectionDelegate {
public:
    virtual ~SbfTcpConnectionDelegate () { }
    
    virtual void onReady () { }

    virtual void onError () { }

    virtual size_t onRead (void* data, size_t size) { return size; }
};

class SbfTcpConnection {
public:
    SbfTcpConnection (sbfLog log,
                      struct sbfMwThreadImpl* thread,
                      struct sbfQueueImpl* queue,
                      sbfTcpConnectionAddress* address,
                      bool isUnix,
                      SbfTcpConnectionDelegate* delegate)
        : mConnection (NULL),
          mLog (log),
          mThread (thread),
          mQueue (queue),
          mAddress (address),
          mIsUnix (isUnix),
          mDelegate (delegate)
    {
    }

    virtual ~SbfTcpConnection ()
    {
        if (getHandle () != NULL)
            sbfTcpConnection_destroy (getHandle ());
    }

    virtual sbfTcpConnection getHandle ()
    {
        return mConnection;
    }

    virtual bool connect ()
    {
        mConnection = sbfTcpConnection_create (mLog,
                                               mThread,
                                               mQueue,
                                               mAddress,
                                               mIsUnix ? 1 : 0,
                                               SbfTcpConnection::sbfTcpConnectionReadyCb,
                                               SbfTcpConnection::sbfTcpConnectionErrorCb,
                                               SbfTcpConnection::sbfTcpConnectionReadCb,
                                               this);
        return mConnection != NULL;
    }

    virtual void send (const void* data, size_t size)
    {
        sbfTcpConnection_send (getHandle (), data, size);
    }

    virtual void sendBuffer (sbfBuffer buffer)
    {
        sbfTcpConnection_sendBuffer (getHandle (), buffer);
    }

    virtual const sbfTcpConnectionAddress* getPeer ()
    {
        return sbfTcpConnection_getPeer (getHandle ());
    }

    virtual bool isUnix ()
    {
        return sbfTcpConnection_isUnix (getHandle ());
    }

protected:
    sbfTcpConnection          mConnection;
    sbfLog                    mLog;
    struct sbfMwThreadImpl*   mThread;
    struct sbfQueueImpl*      mQueue;
    sbfTcpConnectionAddress*  mAddress;
    bool                      mIsUnix;
    SbfTcpConnectionDelegate* mDelegate;
    
private:
    static void sbfTcpConnectionReadyCb (sbfTcpConnection tc, void* closure)
    {
        SbfTcpConnection* instance = static_cast<SbfTcpConnection*>(closure);
        if (instance->mDelegate)
        {
            instance->mDelegate->onReady ();
        }
    }
    
    static void sbfTcpConnectionErrorCb (sbfTcpConnection tc, void* closure)
    {
        SbfTcpConnection* instance = static_cast<SbfTcpConnection*>(closure);
        if (instance->mDelegate)
        {
            instance->mDelegate->onError ();
        }
    }
    
    static size_t sbfTcpConnectionReadCb (sbfTcpConnection tc,
                                          void* data,
                                          size_t size,
                                          void* closure)
    {
        SbfTcpConnection* instance = static_cast<SbfTcpConnection*>(closure);
        if (instance->mDelegate)
        {
            return instance->mDelegate->onRead (data, size);
        }
        return size;
    }
};
}
