/*!
   \file sbfBatch.h
   \brief This file declares the structures and funcions to handle batches.
   \copyright © Copyright 2016 Neueda all rights reserved.
*/
#ifndef _SBF_BATCH_H_
#define _SBF_BATCH_H_

#include "sbfBuffer.h"
#include "sbfCommon.h"

SBF_BEGIN_DECLS

struct sbfTportImpl;
struct sbfPubImpl;

/** Declaration of sbfBatch handler */
typedef struct sbfBatchImpl* sbfBatch;

/*!
   \brief creates a batch associated to a given port.
   \param tport the port handler.
   \return the batch handler.
   \see sbfBatch_create
*/
sbfBatch sbfBatch_create (struct sbfTportImpl* tport);

/*!
   \brief Releases the allocated resources allocated when the batch was
   created.
   \param batch the batch handler.
   \see sbfBatch_create
*/
void sbfBatch_destroy (sbfBatch batch);

/*!
   \brief Adds buffer to the batch.
   A pointer to the publisher is kept in the batch until it is destroyed so
   care must be taken not to destroy the publishers used with a batch efore the
   batch itself.
   \param batch the batch handler.
   \param pub the publisher.
   \param buffer the buffer containing the data to be sent.
*/
void sbfBatch_addBuffer (sbfBatch batch,
                         struct sbfPubImpl* pub,
                         sbfBuffer buffer);

/*!
   \brief Adds buffer to the batch.
   A pointer to the publisher is kept in the batch until it is destroyed so
   care must be taken not to destroy the publishers used with a batch efore the
   batch itself.
   \param batch the batch handler.
   \param pub the publisher.
   \param data the buffer containing the data to be sent.
   \param size the size of the buffer.
*/
void sbfBatch_add (sbfBatch batch,
                   struct sbfPubImpl* pub,
                   void* data,
                   size_t size);

/*!
   \brief Sends the data holded by the given batch handler.
   \param batch the batch handler.
*/
void sbfBatch_send (sbfBatch batch);

SBF_END_DECLS

#endif /* _SBF_BATCH_H_ */
