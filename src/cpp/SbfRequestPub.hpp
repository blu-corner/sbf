#pragma once

#include "sbfRequestReply.h"
#include "SbfTport.hpp"
#include "SbfQueue.hpp"
#include "SbfBuffer.hpp"

namespace neueda
{

class SbfRequestPub;
class SbfRequestPubDelegate {
public:
    virtual ~SbfRequestPubDelegate() { }
    virtual void onPubReady (neueda::SbfRequestPub* pub) { }
    virtual void onPubReply (neueda::SbfRequestPub* pub,
                             struct sbfRequestImpl* req,
                             neueda::SbfBuffer* buf) { }
};

class SbfRequestPub {
public:
    SbfRequestPub (neueda::SbfTport* tport,
                   neueda::SbfQueue* queue,
                   const char* topic,
                   neueda::SbfRequestPubDelegate* delegate)
        : mDelegate (delegate)
    {
        mValue = sbfRequestPub_create (tport->getHandle (),
                                       queue->getHandle (),
                                       topic,
                                       SbfRequestPub::sbfPubReady,
                                       this);
    }

    virtual ~SbfRequestPub ()
    {
        if (getHandle () != NULL)
            sbfRequestPub_destroy (getHandle ());
    }

    virtual sbfRequestPub getHandle ()
    {
        return mValue;
    }

    virtual void send (void* data, size_t size)
    {
        if (getHandle () != NULL)
            sbfRequestPub_send (getHandle (),
                                data,
                                size,
                                SbfRequestPub::sbfPubReply,
                                this);
    }

    virtual const char* getTopic ()
    {
        const char* ret = NULL;
        if (getHandle () != NULL)
            ret = sbfRequestPub_getTopic (getHandle ());
        return ret;
    }

protected:
    sbfRequestPub   mValue;
    neueda::SbfRequestPubDelegate* mDelegate;

private:
    static void sbfPubReady (sbfRequestPub pub, void* closure)
    {
        SbfRequestPub* instance = static_cast<SbfRequestPub*>(closure);

        if (instance->mDelegate)
        {
            instance->mDelegate->onPubReady (instance);
        }
    }

    static void sbfPubReply (sbfRequestPub pub,
                             struct sbfRequestImpl* req,
                             sbfBuffer buffer,
                             void* closure)
    {
        SbfRequestPub* instance = static_cast<SbfRequestPub*>(closure);
        SbfBuffer buf = neueda::SbfBuffer (buffer->mData, buffer->mSize);

        if (instance->mDelegate)
        {
            instance->mDelegate->onPubReply (instance, req, &buf);
        }
    }
};

}
