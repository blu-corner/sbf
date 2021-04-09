/*!
   \file sbfTimer.h
   \brief This file declares the interface for handling queues.
   \Copyright 2014-2018 Neueda Ltd.
*/

#ifndef _SBF_TIMER_H_
#define _SBF_TIMER_H_

#include "sbfCommon.h"

SBF_BEGIN_DECLS


struct sbfMwThreadImpl;
struct sbfQueueImpl;

typedef struct sbfTimerImpl* sbfTimer;

/*!
   \brief callback invoked when timer needs to be processed.
   \param timer the timer handler.
   \param closure the user data associated to the queue item.
*/
typedef void (*sbfTimerCb) (sbfTimer timer, void* closure);

/** \brief The milliseconds for the given seconds. */
#define SBF_TIMER_MILLISECONDS(n) ((n)/1000.0)

/** \brief The micro seconds for the given seconds. */
#define SBF_TIMER_MICROSECONDS(n) ((n)/1000000.0)

/*!
 \brief Enable a timer that was created with sbfTimer_create_disabled
 \param timer the timer handler.
*/
void sbfTimer_enable (sbfTimer timer);

/*!
   \brief Create a timer with the given arguments which can be enabled later.
   \param thread the thread handler.
   \param queue a queue.
   \param cb High resolution timer callback. It will be invoked when the
   timer expires.
   \param closure user data associated to this high resolution timer.
   \param interval the interval, in seconds, the timer is configured to fire
    the callback.
   \return a timer handler.
*/
sbfTimer sbfTimer_create_disabled (struct sbfMwThreadImpl* thread,
                                   struct sbfQueueImpl* queue,
                                   sbfTimerCb cb,
                                   void* closure,
                                   double interval);

/*!
   \brief Create a timer with the given arguments.
   \param thread the thread handler.
   \param queue a queue.
   \param cb High resolution timer callback. It will be invoked when the
   timer expires.
   \param closure user data associated to this high resolution timer.
   \param interval the interval, in seconds, the timer is configured to fire
    the callback.
   \return a timer handler.
*/
sbfTimer sbfTimer_create (struct sbfMwThreadImpl* thread,
                          struct sbfQueueImpl* queue,
                          sbfTimerCb cb,
                          void* closure,
                          double interval);

/*!
 \brief Release the memory allocated by the high resolution timer.
 This also stops the timer so no callback will be raised.
 \param timer the timer handler.
*/
void sbfTimer_destroy (sbfTimer timer);

/*!
   \brief returns the interval the timer was configured.
   \param timer the timer handler.
   \return the interval, in seconds, the timer is configured to fire
    the callback.
*/
double sbfTimer_getInterval (sbfTimer timer);

SBF_END_DECLS

#endif /* _SBF_TIMER_H_ */
