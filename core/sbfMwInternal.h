#ifndef _SBF_MW_INTERNAL_H_
#define _SBF_MW_INTERNAL_H_

#include "sbfMw.h"

#include <event.h>
#include <event2/thread.h>
#include <event2/listener.h>

SBF_BEGIN_DECLS

struct event_base* sbfMw_getThreadEventBase (sbfMwThread thread);

/*
 * Add an event to a thread's event base. This uses event_active to preserve
 * the FIFO behaviour.
 */
static inline void
sbfMw_enqueue (sbfMwThread thread,
               struct event* event,
               event_callback_fn cb,
               void* closure)
{
    event_assign (event,
                  sbfMw_getThreadEventBase (thread),
                  -1,
                  EV_TIMEOUT,
                  cb,
                  closure);
    event_active (event, EV_TIMEOUT, 1);
}

SBF_END_DECLS

#endif
