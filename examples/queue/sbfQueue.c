/*!
   \file sbfQueue.c
   \brief TBD
   \Copyright 2014-2018 Neueda Ltd.
*/
#include "sbfCommon.h"
#include "sbfMw.h"
#include "sbfEvent.h"

static void*
dispatchCb (void* closure)
{
    sbfQueue_dispatch (closure);
    return NULL;
}

static void
queueCb (sbfQueueItem item, void* closure)
{
    printf ("queue callback\n");
}

int
main (int argc, char** argv)
{
    sbfLog      log;
    sbfMw       mw;
    sbfQueue    queue;
    sbfThread   t;
    sbfKeyValue properties;

    log = sbfLog_create (NULL, "%s", "");
    sbfLog_setLevel (log, SBF_LOG_DEBUG);

    properties = sbfKeyValue_create ();
    mw = sbfMw_create (log, properties);
    queue = sbfQueue_create (mw, "default");

    sbfQueue_enqueue (queue, queueCb, NULL);
    sbfQueue_enqueue (queue, queueCb, NULL);
    sbfQueue_enqueue (queue, queueCb, NULL);

    sbfThread_create (&t, dispatchCb, queue);
    
    sbfQueue_destroy (queue);
    sbfThread_join (t);
    sbfMw_destroy (mw);
    sbfKeyValue_destroy (properties);
    sbfLog_destroy (log);

    return 0;
}
