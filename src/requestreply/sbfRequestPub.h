/*!
   \file sbfRequestPub.h
   \brief Helper structures and functions to create a publisher.
   \Copyright 2014-2018 Neueda Ltd.
*/
#ifndef _SBF_REQUEST_PUB_H_
#define _SBF_REQUEST_PUB_H_

#include "sbfCommon.h"
#include "sbfTport.h"
#include "sbfQueue.h"

SBF_BEGIN_DECLS

struct sbfRequestImpl;

/** \brief Declares the publisher */
typedef struct sbfRequestPubImpl* sbfRequestPub;

/*!
   \brief Callback declaration that will be invoked when publisher is ready.
   \param pub the publisher handler.
   \param closure the user data linked to the publisher handler.
*/
typedef void (*sbfRequestPubReadyCb) (sbfRequestPub pub, void* closure);

/*!
   \brief Callback declaration when publisher is replied by the subscriptor.
   \param pub the publisher handler.
   \param req the request handler.
   \param buffer the buffer with the content of the reply.
   \param closure the user data linked to the publisher handler.
*/
typedef void (*sbfRequestPubReplyCb) (sbfRequestPub pub,
                                      struct sbfRequestImpl* req,
                                      sbfBuffer buffer,
                                      void* closure);
/*!
   \brief Creates a publisher configured with the given parameters.
   \param tport the port to initialise the communications.
   \param queue the queue to publish the messages.
   \param topic the logical channel name for the messages.
   \param readyCb the callback indicating that the channel is ready.
   \param closure the user data linked to the publisher handler.
   \return a publisher configured with the given parameters.
*/
sbfRequestPub sbfRequestPub_create (sbfTport tport,
                                    sbfQueue queue,
                                    const char* topic,
                                    sbfRequestPubReadyCb readyCb,
                                    void* closure);
/*!
   \brief Release all the resources allocated by the publisher.
   \param pub the publisher handler.
*/
void sbfRequestPub_destroy (sbfRequestPub pub);

/*!
   \brief Sends a message with the given subscriber.
   \param pub the publisher handler.
   \param data array of bytes to be sent.
   \param size the length in bytes of the data previously defined.
   \param replyCb the reply callback to call when the request is done.
   \param closure the user data linked to the publisher handler.
   \return the request handler.
*/
struct sbfRequestImpl* sbfRequestPub_send (sbfRequestPub pub,
                                           void* data,
                                           size_t size,
                                           sbfRequestPubReplyCb replyCb,
                                           void* closure);

/*!
   \brief returns the name of the topic associated with the given publisher.
   \param pub the publisher handler.
   \return null terminated string containing the name of the topic associated
   with the given publisher.
*/
const char* sbfRequestPub_getTopic (sbfRequestPub pub);

SBF_END_DECLS

#endif /* _SBF_REQUEST_PUB_H_ */
