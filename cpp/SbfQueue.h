#pragma once

#include "sbfQueue.h"

class SbfQueueDelegate
{
public:
    virtual ~SbfQueueDelegate() { }

    virtual void onQueueItem (sbfQueueItem item) { }
};

class SbfQueue
{
public:
    SbfQueue (struct sbfMwImpl* mw,
              const char* name,
              SbfQueueDelegate* delegate);

    virtual ~SbfQueue ();

    sbfQueue getHandle ();

    const char* getName ();

    void enqueue (void* item);

    void enqueueItem (sbfQueueItem item);

    void dispatch ();

    void* getItemData (sbfQueueItem item);

    size_t getItemDataSize (sbfQueueItem item);

private:
    static void sbfQueueCb (sbfQueueItem item, void* closure);
    
    sbfQueue mQueue;
    SbfQueueDelegate* mDelegate;
};
