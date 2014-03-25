#ifndef _SBF_REQUEST_SUB_H_
#define _SBF_REQUEST_SUB_H_

#include "sbfCommon.h"
#include "sbfTport.h"
#include "sbfQueue.h"

SBF_BEGIN_DECLS

struct sbfRequestImpl;

typedef struct sbfRequestSubImpl* sbfRequestSub;

typedef void (*sbfRequestSubReadyCb) (sbfRequestSub sub, void* closure);

/* Request is destroyed unless sbfRequest_detach is called on it. */
typedef void (*sbfRequestSubRequestCb) (sbfRequestSub sub,
                                        struct sbfRequestImpl* req,
                                        sbfBuffer buffer,
                                        void* closure);

sbfRequestSub sbfRequestSub_create (sbfTport tport,
                                    sbfQueue queue,
                                    const char* topic,
                                    sbfRequestSubReadyCb readyCb,
                                    sbfRequestSubRequestCb requestCb,
                                    void* closure);

void sbfRequestSub_destroy (sbfRequestSub sub);

sbfTopic sbfRequestSub_getTopic (sbfRequestSub sub);

void sbfRequestSub_reply (sbfRequestSub sub,
                          struct sbfRequestImpl* req,
                          void* data,
                          size_t size);

SBF_END_DECLS

#endif /* _SBF_REQUEST_SUB_H_ */
