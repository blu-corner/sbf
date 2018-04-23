#include "SbfQueue.h"

SbfQueue::SbfQueue (struct sbfMwImpl* mw,
                    const char* name,
                    SbfQueueDelegate* delegate)
    : mDelegate (delegate)
{
    mQueue = sbfQueue_create (mw, name);
}

SbfQueue::~SbfQueue ()
{
    sbfQueue_destroy (getHandle ());
}

sbfQueue
SbfQueue::getHandle ()
{
    return mQueue;
}

const char*
SbfQueue::getName ()
{
    return sbfQueue_getName (getHandle ());
}

void
SbfQueue::enqueue (void* item)
{
    sbfQueue_enqueue (getHandle (), SbfQueue::sbfQueueCb, item);
}

void
SbfQueue::enqueueItem (sbfQueueItem item)
{
    sbfQueue_enqueueItem (getHandle (), item);
}

void
SbfQueue::dispatch ()
{
    sbfQueue_dispatch (getHandle ());
}

void*
SbfQueue::getItemData (sbfQueueItem item)
{
    return sbfQueue_getItemData (item);
}

size_t
SbfQueue::getItemDataSize (sbfQueueItem item)
{
    return sbfQueue_getItemDataSize (item);
}

void
SbfQueue::sbfQueueCb (sbfQueueItem item, void* closure)
{
    SbfQueueDelegate* delegate = static_cast<SbfQueueDelegate*>(closure);
    delegate->onQueueItem (item);
}
