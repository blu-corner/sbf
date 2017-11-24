/*!
   \file sbfSub.h
   \brief This file declares the structures and functions to handler
   a subscriber.
   \copyright © Copyright 2016 Neueda all rights reserved.
*/
#ifndef _SBF_SUB_H_
#define _SBF_SUB_H_

#include "sbfBuffer.h"
#include "sbfCommon.h"
#include "sbfQueue.h"

SBF_BEGIN_DECLS

struct sbfTportImpl;
struct sbfTopicImpl;

/** Declare the subscriber handler */
typedef struct sbfSubImpl* sbfSub;

/*!
   \brief Callback invoked when subscriber is ready to receive messages.
   \param sub the subscriber's handler.
*/
typedef void (*sbfSubReadyCb) (sbfSub sub, void* closure);

/*!
   \brief Callback invoked when message is received.
   \param sub the subscriber's handler.
   \param buffer buffer containing the message.
   \param closure user data linked to the subscriber.
*/
typedef void (*sbfSubMessageCb) (sbfSub sub, sbfBuffer buffer, void* closure);

/*!
   \brief Creates a subscriber for a given port and topic.
   \param tport the connection port linked to the transport interface.
   \param queue the queue used to process the received messages.
   \param topic the topic associated to the connection.
   \param readyCb callback invoked when publisher is ready to receive messages.
   \param messageCb callback invoked when message is received.
   \param closure user data linked to the subscriber.
   \return a subscriber for a given port and topic.
*/
sbfSub sbfSub_create (struct sbfTportImpl* tport,
                      sbfQueue queue,
                      const char* topic,
                      sbfSubReadyCb readyCb,
                      sbfSubMessageCb messageCb,
                      void* closure);

/*!
   \brief Releases the resources allocated by the subscriber.
   \param sub the subscriber's handler.
*/
void sbfSub_destroy (sbfSub sub);

/*!
   \brief invoke the callback for the incoming messages without using the queue.
   \param sub the subscriber's handler.
   \param set 0 to use the queue whatever other value to bypass it.
*/
void sbfSub_setQueueBypass (sbfSub sub, int set);

/*!
   \brief Returns the topic associated to this subscriber.
   \param sub the subscriber's handler.
   \return the topic associated to this subscriber.
*/
struct sbfTopicImpl* sbfSub_getTopic (sbfSub sub);

SBF_END_DECLS

#endif /* _SBF_SUB_H_ */
