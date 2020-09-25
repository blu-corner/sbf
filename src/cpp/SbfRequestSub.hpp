#pragma once

#include "sbfRequestReply.h"
#include "SbfBuffer.hpp"
#include "SbfTport.hpp"
#include "SbfQueue.hpp"

namespace neueda
{

class SbfRequestSub;
class SbfRequestSubDelegate {
public:
    virtual ~SbfRequestSubDelegate() { }
    virtual void onSubReady (neueda::SbfRequestSub* sub) { }
    virtual void onSubRequest (neueda::SbfRequestSub* sub,
                               struct sbfRequestImpl* req,
                               neueda::SbfBuffer* buf) { }
};

class SbfRequestSub {
public:
    SbfRequestSub (neueda::SbfTport* tport,
                   neueda::SbfQueue* queue,
                   const char* topic,
                   neueda::SbfRequestSubDelegate* delegate)
        : mDelegate (delegate)
    {
        mValue = sbfRequestSub_create (tport->getHandle (),
                                       queue->getHandle (),
                                       topic,
                                       SbfRequestSub::sbfSubReady,
                                       SbfRequestSub::sbfSubRequest,
                                       this);
    }

    virtual ~SbfRequestSub ()
    {
        if (getHandle () != NULL)
            sbfRequestSub_destroy (getHandle ());
    }

    virtual sbfRequestSub getHandle ()
    {
        return mValue;
    }

    virtual void reply (struct sbfRequestImpl* req, void* data, size_t size)
    {
        if (getHandle () != NULL)
            sbfRequestSub_reply (getHandle(), req, data, size);
    }

    virtual const char* getTopic ()
    {
        const char* ret = NULL;
        if (getHandle () != NULL)
            ret = sbfRequestSub_getTopic (getHandle ());
        return ret;
    }

protected:
    sbfRequestSub   mValue;
    neueda::SbfRequestSubDelegate* mDelegate;

private:
    static void sbfSubReady (sbfRequestSub sub, void* closure)
    {
        SbfRequestSub* instance = static_cast<SbfRequestSub*>(closure);

        if (instance->mDelegate)
        {
            instance->mDelegate->onSubReady (instance);
        }
    }

    static void sbfSubRequest (sbfRequestSub sub,
                               struct sbfRequestImpl* req,
                               sbfBuffer buffer,
                               void* closure)
    {
        SbfRequestSub* instance = static_cast<SbfRequestSub*>(closure);
        SbfBuffer buf = neueda::SbfBuffer (buffer->mData, buffer->mSize);

        if (instance->mDelegate)
        {
            instance->mDelegate->onSubRequest (instance, req, &buf);
        }
    }
};

}
