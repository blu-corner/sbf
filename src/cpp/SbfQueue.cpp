#include "SbfQueue.hpp"

neueda::SbfQueue::SbfQueue (SbfMw* mw, const char* name)
{
    mQueue = sbfQueue_create(mw->getHandle(), name);
}

neueda::SbfQueue::~SbfQueue ()
{
    if (getHandle () != NULL)
        sbfQueue_destroy (getHandle ());
}

void neueda::SbfQueue::destroy()
{
    if (getHandle () != NULL)
        sbfQueue_destroy (getHandle ());
}

sbfQueue neueda::SbfQueue::getHandle ()
{
    return mQueue;
}

const char* neueda::SbfQueue::getName ()
{
    const char* ret = NULL;
    if (getHandle () != NULL)
    {
        ret = sbfQueue_getName (getHandle ());
    }
    return ret;
}

void neueda::SbfQueue::enqueue (SbfQueueDelegate* item)
{
    sbfQueue_enqueue (getHandle (), SbfQueue::sbfQueueCb, item);
}

void neueda::SbfQueue::dispatch ()
{
    sbfQueue_dispatch (mQueue);
}
