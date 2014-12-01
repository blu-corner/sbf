#ifndef _SBF_EVENT_H_
#define _SBF_EVENT_H_

#include "sbfCommon.h"

SBF_BEGIN_DECLS

struct sbfMwThreadImpl;
struct sbfQueueImpl;

typedef struct sbfEventImpl* sbfEvent;

#define SBF_EVENT_READ 0x2
#define SBF_EVENT_WRITE 0x4

typedef void (*sbfEventCb) (sbfEvent event, int events, void* closure);

/* Events is one or both of SBF_EVENT_READ and SBF_EVENT_WRITE. */
sbfEvent sbfEvent_create (struct sbfMwThreadImpl* thread,
                          struct sbfQueueImpl* queue,
                          sbfEventCb cb,
                          void* closure,
                          int descriptor,
                          int events);
void sbfEvent_destroy (sbfEvent event);

SBF_END_DECLS

#endif /* _SBF_EVENT_H_ */
