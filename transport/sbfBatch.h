#ifndef _SBF_BATCH_H_
#define _SBF_BATCH_H_

#include "sbfBuffer.h"
#include "sbfCommon.h"

SBF_BEGIN_DECLS

struct sbfTportImpl;
struct sbfPubImpl;

typedef struct sbfBatchImpl* sbfBatch;

sbfBatch sbfBatch_create (struct sbfTportImpl* tport);
void sbfBatch_destroy (sbfBatch batch);

/*
 * A pointer to the publisher is kept in the batch until it is destroyed so
 * care must be taken not to destroy the publishers used with a batch efore the
 * batch itself.
 */
void sbfBatch_addBuffer (sbfBatch batch,
                         struct sbfPubImpl* pub,
                         sbfBuffer buffer);
void sbfBatch_add (sbfBatch batch,
                   struct sbfPubImpl* pub,
                   void* data,
                   size_t size);

void sbfBatch_send (sbfBatch batch);

SBF_END_DECLS

#endif /* _SBF_BATCH_H_ */
