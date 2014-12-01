#ifndef _SBF_PUB_H_
#define _SBF_PUB_H_

#include "sbfBuffer.h"
#include "sbfCommon.h"
#include "sbfQueue.h"

SBF_BEGIN_DECLS

struct sbfTportImpl;
struct sbfTopicImpl;

typedef struct sbfPubImpl* sbfPub;

typedef void (*sbfPubReadyCb) (sbfPub pub, void* closure);

sbfPub sbfPub_create (struct sbfTportImpl* tport,
                      sbfQueue queue,
                      const char* topic,
                      sbfPubReadyCb readyCb,
                      void* closure);
void sbfPub_destroy (sbfPub pub);

/*
 * sendBuffer destroys the buffer. It must not have more than one reference.
 * Messages that are too big and those sent before the ready callback are
 * silently dropped.
 */
sbfBuffer sbfPub_getBuffer (sbfPub pub, size_t size);
void sbfPub_sendBuffer (sbfPub pub, sbfBuffer buffer);
void sbfPub_send (sbfPub pub, void* data, size_t size);

struct sbfTopicImpl* sbfPub_getTopic (sbfPub pub);

SBF_END_DECLS

#endif /* _SBF_PUB_H_ */
