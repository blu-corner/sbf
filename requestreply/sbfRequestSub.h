/*!
   \file sbfRequestSub.h
   \brief Helper structures and functions to create a subscriber.
   \copyright © Copyright 2016 Neueda all rights reserved.
*/
#ifndef _SBF_REQUEST_SUB_H_
#define _SBF_REQUEST_SUB_H_

#include "sbfCommon.h"
#include "sbfTport.h"
#include "sbfQueue.h"

SBF_BEGIN_DECLS

struct sbfRequestImpl;

/** \brief The subscriber declaration */
typedef struct sbfRequestSubImpl* sbfRequestSub;

/*!
   \brief Callback declaration that will be invoked when subscriber is ready.
   \param sub the subscriber handler.
   \param closure the user data linked to the subscriber handler.
*/
typedef void (*sbfRequestSubReadyCb) (sbfRequestSub sub, void* closure);

/*!
   \brief Callback declaration that will be invoked when request is processed.
   Request is destroyed unless sbfRequest_detach is called on it.
   \param sub the subscriber handler.
   \param req the request that is being attended.
   \param buffer the buffer requested.
   \param closure the user data linked to the subscriber handler.
*/
typedef void (*sbfRequestSubRequestCb) (sbfRequestSub sub,
                                        struct sbfRequestImpl* req,
                                        sbfBuffer buffer,
                                        void* closure);

/*!
   \brief Creates a subscriber configured with the given parameters.
   \param tport the port to initialise the communications.
   \param queue the queue to sublish the messages.
   \param topic the logical channel name for the messages.
   \param readyCb the callback indicating that the channel is ready.
   \param requestCb the callback indicating that the channel is ready.
   \param closure the user data linked to the subscriber handler.
   \return a subscriber configured with the given parameters.
*/
sbfRequestSub sbfRequestSub_create (sbfTport tport,
                                    sbfQueue queue,
                                    const char* topic,
                                    sbfRequestSubReadyCb readyCb,
                                    sbfRequestSubRequestCb requestCb,
                                    void* closure);

/*!
   \brief Release all the resources allocated by the subscriber.
   \param sub the subscriber handler.
*/
void sbfRequestSub_destroy (sbfRequestSub sub);

/*!
   \brief returns the name of the topic associated with the given subscriber.
   \param sub the subscriber handler.
   \return null terminated string containing the name of the topic associated
   with the given subscriber.
*/
const char* sbfRequestSub_getTopic (sbfRequestSub sub);

/*!
   \brief Sends a message with the given subscriber.
   \param sub the subscriber handler.
   \param req the request that is being replied.
   \param data array of bytes to be sent.
   \param size the length in bytes of the data previously defined.
*/
void sbfRequestSub_reply (sbfRequestSub sub,
                          struct sbfRequestImpl* req,
                          void* data,
                          size_t size);

SBF_END_DECLS

#endif /* _SBF_REQUEST_SUB_H_ */
