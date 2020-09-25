#pragma once

#include "sbfPub.h"
#include "SbfTport.hpp"
#include "SbfQueue.hpp"
#include "SbfBuffer.hpp"

namespace neueda
{

class SbfPub;
class SbfPubDelegate {
public:
    virtual ~SbfPubDelegate() { }
    virtual void onPubReady (SbfPub* pub) { }
};

class SbfPub {
public:
    SbfPub (neueda::SbfTport* tport,
            neueda::SbfQueue* queue,
            const char* topic,
            neueda::SbfPubDelegate* delegate)
        : mDelegate (delegate)
    {
        mValue = sbfPub_create (tport->getHandle (),
                                queue->getHandle (),
                                topic,
                                SbfPub::sbfPubReady,
                                this);
    }

    virtual ~SbfPub ()
    {
        if (getHandle () != NULL)
            sbfPub_destroy (getHandle ());
    }

    virtual sbfPub getHandle ()
    {
        return mValue;
    }

    virtual neueda::SbfBuffer* getBuffer (size_t size)
    {
        SbfBuffer* ret = NULL;
        if (getHandle () != NULL)
        {
            sbfBuffer buf = sbfPub_getBuffer (getHandle (), size);
            ret = new neueda::SbfBuffer (buf->mData, buf->mSize);
        }
        return ret;
    }

    virtual void sendBuffer (neueda::SbfBuffer* buffer)
    {
        if (getHandle () != NULL)
        {
            sbfPub_sendBuffer (getHandle (), buffer->getHandle ());
            delete buffer;
        }
    }
    
    virtual void send (void* data, size_t size)
    {
        if (getHandle () != NULL)
        {
            sbfPub_send (getHandle (), data, size);
        }
    }

    virtual sbfTopicImpl* getTopic ()
    {
        sbfTopicImpl* ret = NULL;
        if (getHandle () != NULL)
        {
            ret = sbfPub_getTopic (getHandle ());
        }
        return ret;
    }

protected:
    sbfPub          mValue;
    SbfPubDelegate* mDelegate;

private:
    static void sbfPubReady (sbfPub pub, void* closure)
    {
        SbfPub* instance = static_cast<SbfPub*>(closure);

        if (instance->mDelegate)
        {
            instance->mDelegate->onPubReady (instance);
        }
    }
};

}
