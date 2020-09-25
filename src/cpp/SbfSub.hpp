#pragma once

#include "sbfSub.h"
#include "SbfTport.hpp"
#include "SbfQueue.hpp"
#include "SbfBuffer.hpp"

namespace neueda
{

class SbfSub;
class SbfSubDelegate {
public:
    virtual ~SbfSubDelegate() { }
    virtual void onSubReady (neueda::SbfSub* sub) { }
    virtual void onSubMessage (neueda::SbfSub* sub,
                               neueda::SbfBuffer* buffer) { }
};

class SbfSub {
public:
    SbfSub (neueda::SbfTport* tport,
            neueda::SbfQueue* queue,
            const char* topic,
            neueda::SbfSubDelegate* delegate)
        : mDelegate (delegate)
    {
        mValue = sbfSub_create (tport->getHandle (),
                                queue->getHandle (),
                                topic,
                                SbfSub::sbfSubReady,
                                SbfSub::sbfSubMessage,
                                this);
    }

    virtual ~SbfSub ()
    {
        if (getHandle () != NULL)
            sbfSub_destroy (getHandle ());
    }

    virtual sbfSub getHandle ()
    {
        return mValue;
    }

    virtual sbfTopicImpl* getTopic ()
    {
        sbfTopicImpl* ret = NULL;
        if (getHandle () != NULL)
            ret = sbfSub_getTopic (getHandle ());
        return ret;
    }

protected:
    sbfSub          mValue;
    neueda::SbfSubDelegate* mDelegate;

private:
    static void sbfSubReady (sbfSub sub, void* closure)
    {
        SbfSub* instance = static_cast<SbfSub*>(closure);

        if (instance->mDelegate)
        {
            instance->mDelegate->onSubReady (instance);
        }
    }

    static void sbfSubMessage (sbfSub sub, sbfBuffer buffer, void* closure)
    {
        SbfSub* instance = static_cast<SbfSub*>(closure);
        SbfBuffer buf = neueda::SbfBuffer (buffer->mData, buffer->mSize);

        if (instance->mDelegate)
        {
            instance->mDelegate->onSubMessage (instance, &buf);
        }
    }
};

}
