/*!
   \file sbfPub.h
   \brief This file declares the structures and functions to handler
   a publisher.
   \Copyright 2014-2018 Neueda Ltd.
*/
#ifndef _SBF_PUB_H_
#define _SBF_PUB_H_

#include "sbfBuffer.h"
#include "sbfCommon.h"
#include "sbfQueue.h"

SBF_BEGIN_DECLS

struct sbfTportImpl;
struct sbfTopicImpl;

typedef struct sbfPubImpl* sbfPub;

/*!
   \brief Callback invoked when publisher is ready.
   \param pub the publisher's handler.
   \param closure user data linked to the publisher's handler.
*/
typedef void (*sbfPubReadyCb) (sbfPub pub, void* closure);

/*!
   \brief Creates a publisher for a given port and topic.
   \param tport connection port.
   \param queue queue.
   \param topic the topic.
   \param readyCb the callback indicating the the publisher is ready.
   \param closure user data linked to the publisher's handler.
   \return Handler to publisher.
*/
sbfPub sbfPub_create (struct sbfTportImpl* tport,
                      sbfQueue queue,
                      const char* topic,
                      sbfPubReadyCb readyCb,
                      void* closure);

/*!
   \brief Releases all the resources allocated during the creation of the
   publisher.
   \param pub the publisher's handler.
*/
void sbfPub_destroy (sbfPub pub);

 /*!
    \brief Allocates and returns a buffer for a given publisher.
    \param pub the publisher's handler.
    \param size the size of the buffer to be allocated.
    \return the buffer allocated by the publisher.
 */
sbfBuffer sbfPub_getBuffer (sbfPub pub, size_t size);

/*!
   \brief Sends a buffer throught the given publisher.
   SendBuffer destroys the buffer. It must not have more than one reference.
   Messages that are too big and those sent before the ready callback are
   silently dropped.
   \param pub the publisher's handler.
   \param buffer the buffer to be sent.
*/
void sbfPub_sendBuffer (sbfPub pub, sbfBuffer buffer);

/*!
   \brief Sends a buffer throught the given publisher.
   SendBuffer destroys the buffer. It must not have more than one reference.
   Messages that are too big and those sent before the ready callback are
   silently dropped.
   \param pub the publisher's handler.
   \param data pointer to the payload to be sent.
   \param size the size of the payload.
*/
void sbfPub_send (sbfPub pub, void* data, size_t size);

/*!
   \brief Returns the topic associated to the publisher.
   \param pub the publisher's handler.
   \return the topic associated to the publisher.
*/
struct sbfTopicImpl* sbfPub_getTopic (sbfPub pub);

SBF_END_DECLS

#endif /* _SBF_PUB_H_ */
