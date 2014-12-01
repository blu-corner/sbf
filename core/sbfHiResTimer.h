#ifndef _SBF_HI_RES_TIMER_H_
#define _SBF_HI_RES_TIMER_H_

#include "sbfCommon.h"

SBF_BEGIN_DECLS

struct sbfQueueImpl;

typedef struct sbfHiResTimerImpl* sbfHiResTimer;

typedef void (*sbfHiResTimerCb) (sbfHiResTimer timer, void* closure);

sbfHiResTimer sbfHiResTimer_create (struct sbfQueueImpl* queue,
                                    sbfHiResTimerCb cb,
                                    void* closure,
                                    u_int microseconds);
void sbfHiResTimer_destroy (sbfHiResTimer timer);

u_int sbfHiResTimer_getInterval (sbfHiResTimer timer);

SBF_END_DECLS

#endif /* _SBF_HI_RES_TIMER_H_ */
