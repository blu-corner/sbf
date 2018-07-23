#pragma once

#include "sbfUdpMulticast.h"

namespace neueda
{

class SbfUdpMulticastConnectionDelegate {
public:
    virtual ~SbfUdpMulticastConnectionDelegate () { }

    virtual sbfError onRead (sbfBuffer buffer) { return 0; }

};

class SbfUdpMulticastConnection {
public:
    SbfUdpMulticastConnection (sbfUdpMulticastType type,
                               uint32_t interf,
                               struct socketaddr_in* address
                               SbfUdpMulticastConnectionDelegate* delegate)
        : mType (type),
          mInterf (interf),
          mAddress (address)
          mDelegate (delegate)

    {}

    virtual ~SbfUdpMulticastConnection ()
    {
        if (getHandle () != NULL)
            sbfUdpMulticast_destroy (getHandle ());
    }

    virtual sbfUdpMulticast getHandle ()
    {
        return mConnection;
    }

    virtual bool connect ()
    {
        mConnection = sbfUdpMulticast_create (mType, mInterf, mAddress);

        return mConnection != NULL;
    }

    virtual sbfSocket getSocket ()
    {
        return sbfUdpMulticast_getSocket (getHandle ());
    }

    virtual sbfError send (const void* data, size_t size)
    {
        return sbfUdpMulticast_send (getHandle (), data, size);
    }

    virtual sbfError sendBuffer (sbfBuffer buffer)
    {
        return sbfUdpMulticast_sendBuffer (getHandle (), buffer);
    }

    virtual sbfError read (u_int limit)
    {
        return sbfUdpMulticast_read (
                              getHandle (),
                              SbfUdpMulticastConnection::sbfUdpConnectionRead,
                              this,
                              limit)
    }

protected:
    sbfUdpMulticast                    mConnection;
    sbfUdpMulticastType                mType;
    uint32_t                           mInterf;
    struct socketaddr_in*              mAddress;
    SbfUdpMulticastConnectionDelegate* mDelegate;

private:
    static sbfError sbfUdpConnectionRead (sbfUdpMulticast uc,
                                          sbfBuffer buffer,
                                          void* closure)
    {
        SbfUdpMulticastConnection* instance = static_cast<SbfUdpMulticastConnection*>(closure);
        if (instance->mDelegate)
        {
            return instance->mDelegate->onRead(buffer);
        }

        return 1;
    }

};
}
