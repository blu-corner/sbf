#pragma once

#include "sbfTcpConnection.h"

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
                      SbfTcpConnectionDelegate* delegate);

    virtual ~SbfTcpConnection ();

    virtual sbfTcpConnection getHandle ();

    virtual void send (const void* data, size_t size);

    virtual void sendBuffer (sbfBuffer buffer);

    virtual const sbfTcpConnectionAddress* getPeer ();

    virtual bool isUnix ();

protected:
    sbfTcpConnection mConnection;
    SbfTcpConnectionDelegate* mDelegate;
    
private:
    static void sbfTcpConnectionReadyCb (sbfTcpConnection tc, void* closure);
    static void sbfTcpConnectionErrorCb (sbfTcpConnection tc, void* closure);
    static size_t sbfTcpConnectionReadCb (sbfTcpConnection tc,
                                          void* data,
                                          size_t size,
                                          void* closure);
};

