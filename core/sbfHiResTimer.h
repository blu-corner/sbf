/*!
   \file sbfHiResTimer.h
   \brief This file declares the structures and functions to provide high
   resolution timers.
   \Copyright 2014-2018 Neueda Ltd.
*/
#ifndef _SBF_HI_RES_TIMER_H_
#define _SBF_HI_RES_TIMER_H_

#include "sbfCommon.h"

SBF_BEGIN_DECLS

struct sbfQueueImpl;

typedef struct sbfHiResTimerImpl* sbfHiResTimer;

/*!
   \brief Callback invoked when timer expires.
   \param timer the timer handler.
   \param closure the user data associated to the timer.
*/
typedef void (*sbfHiResTimerCb) (sbfHiResTimer timer, void* closure);

/*!
   \brief Returns a high resolution timer handler
   initialised with the given data.
   \param queue a queue.
   \param cb High resolution timer callback. It will be invoked when the
   timer expires.
   \param closure user data associated to this high resolution timer.
   \param microseconds the microseconds to fire the timer.
   \return a high resolution timer handler.
*/
sbfHiResTimer sbfHiResTimer_create (struct sbfQueueImpl* queue,
                                    sbfHiResTimerCb cb,
                                    void* closure,
                                    u_int microseconds);

/*!
   \brief Release the memory allocated by the high resolution timer.
   This also stops the timer so no callback will be raised.
   \param timer the timer handler.
*/
void sbfHiResTimer_destroy (sbfHiResTimer timer);

/*!
   \brief "Description"
   \param timer the timer handler.
   \return "Return of the function"
*/
u_int sbfHiResTimer_getInterval (sbfHiResTimer timer);

SBF_END_DECLS

#endif /* _SBF_HI_RES_TIMER_H_ */
