#ifndef _SBF_TIMER_H_
#define _SBF_TIMER_H_

#include "sbfCommon.h"

SBF_BEGIN_DECLS

struct sbfMwThreadImpl;
struct sbfQueueImpl;

typedef struct sbfTimerImpl* sbfTimer;

typedef void (*sbfTimerCb) (sbfTimer timer, void* closure);

sbfTimer sbfTimer_create (struct sbfMwThreadImpl* thread,
                          struct sbfQueueImpl* queue,
                          sbfTimerCb cb,
                          void* closure,
                          double interval);
void sbfTimer_destroy (sbfTimer timer);

double sbfTimer_getInterval (sbfTimer timer);

SBF_END_DECLS

#endif /* _SBF_TIMER_H_ */
