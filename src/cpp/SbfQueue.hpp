#pragma once

#include "sbfQueue.h"
#include "SbfMw.hpp"

namespace neueda
{

class SbfQueueDelegate
{
public:
    virtual ~SbfQueueDelegate() { }

    virtual void onQueueItem () { }
};

class SbfQueue
{
public:
    SbfQueue (SbfMw* mw, const char* name)
    {
        mQueue = sbfQueue_create(mw->getHandle(), name);
    }

    virtual ~SbfQueue ()
    {
        if (getHandle () != NULL)
            sbfQueue_destroy (getHandle ());
    }

    virtual void destroy()
    {
        if (getHandle () != NULL)
            sbfQueue_destroy (getHandle ());
    }

    virtual sbfQueue getHandle ()
    {
        return mQueue;
    }

    const char* getName ()
    {
        const char* ret = NULL;
        if (getHandle () != NULL)
        {
            ret = sbfQueue_getName (getHandle ());
        }
        return ret;
    }

    void enqueue (SbfQueueDelegate* item)
    {
        sbfQueue_enqueue (getHandle (), SbfQueue::sbfQueueCb, item);
    }

    void dispatch ()
    {
        sbfQueue_dispatch (mQueue);
    }

private:
    static void sbfQueueCb (sbfQueueItem queueItem, void* closure)
    {
        SbfQueueDelegate* instance = static_cast<SbfQueueDelegate*>(closure);

        if (instance)
        {
            instance->onQueueItem ();
        }
    }
    
    sbfQueue mQueue;
};

}
