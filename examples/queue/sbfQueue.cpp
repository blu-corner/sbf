/*!
   \file sbfQueue.cpp
   \brief TBD
   \Copyright 2014-2018 Neueda Ltd.
*/
#include "sbfMw.h"
#include "sbfEvent.h"
#include "SbfQueue.hpp"

static void*
dispatchCb (void* closure)
{
    neueda::SbfQueue* instance = static_cast<neueda::SbfQueue*>(closure);
    instance->dispatch();
    return NULL;   
}

class QueueDelegate1 : public neueda::SbfQueueDelegate
{
public:
    virtual void onQueueItem ()
    {
        printf ("Queue callback 1\n");
    }
};

class QueueDelegate2 : public neueda::SbfQueueDelegate
{
public:
    virtual void onQueueItem ()
    {
        printf ("Queue callback 2\n");
    }
};

int
main (int argc, char** argv)
{
    neueda::SbfLog*      log;
    neueda::SbfMw*       mw;
    neueda::SbfQueue*    queue;
    neueda::SbfKeyValue* properties;
    sbfThread    t;

    log = new neueda::SbfLog ();
    log->setLevel (SBF_LOG_DEBUG);
    log->log (SBF_LOG_INFO, "%s", "Queue example started");

    properties  = new neueda::SbfKeyValue ();

    mw = new neueda::SbfMw (log, properties);
    queue = new neueda::SbfQueue (mw, "default");

    QueueDelegate1* delegate1 = new QueueDelegate1();
    QueueDelegate2* delegate2 = new QueueDelegate2();
    queue->enqueue (delegate1);
    queue->enqueue (delegate1);
    queue->enqueue (delegate2);
    queue->enqueue (delegate1);

    sbfThread_create (&t, dispatchCb, queue);
    
    delete queue;
    sbfThread_join (t);
    delete mw;
    delete properties;
    delete log;
    delete delegate1;
    delete delegate2;

    return 0;
}
