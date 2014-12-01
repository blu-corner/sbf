#ifndef _SBF_QUEUE_H_
#define _SBF_QUEUE_H_

#include "sbfCommon.h"

SBF_BEGIN_DECLS

struct sbfMwImpl;

typedef struct sbfQueueImpl* sbfQueue;

typedef struct sbfQueueItemImpl* sbfQueueItem;

typedef void (*sbfQueueCb) (sbfQueueItem item, void* closure);

sbfQueue sbfQueue_create (struct sbfMwImpl* mw,
                          const char* name,
                          void* closure);
void sbfQueue_destroy (sbfQueue queue);

const char* sbfQueue_getName (sbfQueue queue);

void sbfQueue_enqueue (sbfQueue queue, sbfQueueCb cb, void* closure);
void sbfQueue_dispatch (sbfQueue queue);

sbfQueueItem sbfQueue_getItem (sbfQueue queue, sbfQueueCb cb, void* closure);
void sbfQueue_enqueueItem (sbfQueue queue, sbfQueueItem item);

/*
 * Each item has a small data buffer associated with it, to avoid having to
 * allocate a container object for callbacks which need multiple arguments.
 */
void* sbfQueue_getItemData (sbfQueueItem item);
size_t sbfQueue_getItemDataSize (sbfQueueItem item);

SBF_END_DECLS

#endif /* _SBF_QUEUE_H_ */
