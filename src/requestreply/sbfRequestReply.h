/*!
   \file sbfRequestReply.h
   \brief This file declares the structures and functions for the
   requester-replier communication.
*/
#ifndef _SBF_REQUEST_REPLY_H_
#define _SBF_REQUEST_REPLY_H_

#include "sbfCommon.h"
#include "sbfRequestSub.h"
#include "sbfRequestPub.h"
#include "sbfMw.h"
#include "sbfTopic.h"

SBF_BEGIN_DECLS

/** \brief Header size in bytes for reply */
#define SBF_REQUEST_REPLY_HEADER_SIZE 64

/** \brief request handler */
typedef struct sbfRequestImpl* sbfRequest;

/*!
   \brief Replies to a given requester with a given message.
   \param req the request handler.
   \param data message payload in raw byte array.
   \param size the size of the payload.
*/
void sbfRequest_reply (sbfRequest req, void* data, size_t size);

/*!
   \brief Detaches a request so it can be used outside callback.
   \param req the request handler.
*/
void sbfRequest_detach (sbfRequest req);

/*!
   \brief Release all the resources allocated by the request handler.
   \param req the request handler.
*/
void sbfRequest_destroy (sbfRequest req);

SBF_END_DECLS

#endif /* _SBF_REQUEST_REPLY_H_ */
