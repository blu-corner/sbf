/*!
   \file sbfEvent.h
   \brief This file declares the event structures and functions for event
   handling.
   \copyright © Copyright 2016 Neueda all rights reserved.
*/
#ifndef _SBF_EVENT_H_
#define _SBF_EVENT_H_

#include "sbfCommon.h"

SBF_BEGIN_DECLS

//------------------------------------------------------------------------------
// Forward declarations
//------------------------------------------------------------------------------
struct sbfMwThreadImpl;
struct sbfQueueImpl;

/** \brief public event structure */
typedef struct sbfEventImpl* sbfEvent;

/** \brief Event indicating a read operation */
#define SBF_EVENT_READ 0x2
/** \brief Event indicating a write operation */
#define SBF_EVENT_WRITE 0x4

/*!
   \brief Callback function indicating there are events to process.
   \param event Pointer to events objects.
   \param the number of events.
   \param closure the data associated to the callback.
*/
typedef void (*sbfEventCb) (sbfEvent event, int events, void* closure);

/* . */
/*!
   \brief Returns an event handler with the provided information.
   Events is one or both of SBF_EVENT_READ and SBF_EVENT_WRITE.
   \param thread the thread handling the reception of events.
   \param queue queue where events are stored.
   \param cb the event's callback.
   \param closure a data to be associated to the event to be used in callbacks.
   \param descriptor the descriptor.
   \param events the number of events.
   \return an event handler with the provided information.
*/
sbfEvent sbfEvent_create (struct sbfMwThreadImpl* thread,
                          struct sbfQueueImpl* queue,
                          sbfEventCb cb,
                          void* closure,
                          int descriptor,
                          int events);

/*!
   \brief Releases the memory allocated by the given memory handler.
   \param event the event handler.
*/
void sbfEvent_destroy (sbfEvent event);

SBF_END_DECLS

#endif /* _SBF_EVENT_H_ */
