#ifndef _SBF_REQUEST_PUB_H_
#define _SBF_REQUEST_PUB_H_

#include "sbfCommon.h"
#include "sbfTport.h"
#include "sbfQueue.h"

SBF_BEGIN_DECLS

struct sbfRequestImpl;

typedef struct sbfRequestPubImpl* sbfRequestPub;

typedef void (*sbfRequestPubReadyCb) (sbfRequestPub pub, void* closure);

typedef void (*sbfRequestPubReplyCb) (sbfRequestPub pub,
                                      struct sbfRequestImpl* req,
                                      sbfBuffer buffer,
                                      void* closure);

sbfRequestPub sbfRequestPub_create (sbfTport tport,
                                    sbfQueue queue,
                                    const char* topic,
                                    sbfRequestPubReadyCb readyCb,
                                    void* closure);
void sbfRequestPub_destroy (sbfRequestPub pub);

struct sbfRequestImpl* sbfRequestPub_send (sbfRequestPub pub,
                                           void* data,
                                           size_t size,
                                           sbfRequestPubReplyCb replyCb,
                                           void* closure);

const char* sbfRequestPub_getTopic (sbfRequestPub pub);

SBF_END_DECLS

#endif /* _SBF_REQUEST_PUB_H_ */
