#include "SbfTcpConnection.h"

SbfTcpConnection::SbfTcpConnection (sbfLog log,
                                    struct sbfMwThreadImpl* thread,
                                    struct sbfQueueImpl* queue,
                                    sbfTcpConnectionAddress* address,
                                    bool isUnix,
                                    SbfTcpConnectionDelegate* delegate)
    : mDelegate (delegate)
{
    mConnection = sbfTcpConnection_create (log,
                                           thread,
                                           queue,
                                           address,
                                           isUnix ? 1 : 0,
                                           SbfTcpConnection::sbfTcpConnectionReadyCb,
                                           SbfTcpConnection::sbfTcpConnectionErrorCb,
                                           SbfTcpConnection::sbfTcpConnectionReadCb,
                                           this);
}

SbfTcpConnection::~SbfTcpConnection ()
{
    sbfTcpConnection_destroy (getHandle ());
}

sbfTcpConnection
SbfTcpConnection::getHandle ()
{
    return mConnection;
}

void
SbfTcpConnection::send (const void* data, size_t size)
{
    sbfTcpConnection_send (getHandle (), data, size);
}

void
SbfTcpConnection::sendBuffer (sbfBuffer buffer)
{
    sbfTcpConnection_sendBuffer (getHandle (), buffer);
}

const sbfTcpConnectionAddress*
SbfTcpConnection::getPeer ()
{
    return sbfTcpConnection_getPeer (getHandle ());
}

bool
SbfTcpConnection::isUnix ()
{
    return sbfTcpConnection_isUnix (getHandle ());
}

void
SbfTcpConnection::sbfTcpConnectionReadyCb (sbfTcpConnection tc, void* closure)
{
    SbfTcpConnection* instance = static_cast<SbfTcpConnection*>(closure);
    if (instance->mDelegate)
    {
        instance->mDelegate->onReady ();
    }
}

void
SbfTcpConnection::sbfTcpConnectionErrorCb (sbfTcpConnection tc, void* closure)
{
    SbfTcpConnection* instance = static_cast<SbfTcpConnection*>(closure);
    if (instance->mDelegate)
    {
        instance->mDelegate->onError ();
    }
}

size_t
SbfTcpConnection::sbfTcpConnectionReadCb (sbfTcpConnection tc,
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
