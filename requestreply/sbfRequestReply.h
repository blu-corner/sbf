#ifndef _SBF_REQUEST_REPLY_H_
#define _SBF_REQUEST_REPLY_H_

#include "sbfCommon.h"
#include "sbfRequestSub.h"
#include "sbfRequestPub.h"
#include "sbfMw.h"
#include "sbfTopic.h"

SBF_BEGIN_DECLS

#define SBF_REQUEST_REPLY_HEADER_SIZE 64

typedef struct sbfRequestImpl* sbfRequest;

void sbfRequest_reply (sbfRequest req, void* data, size_t size);

/* Detach request so it can be used outside callback. */
void sbfRequest_detach (sbfRequest req);

void sbfRequest_destroy (sbfRequest req);

SBF_END_DECLS

#endif /* _SBF_REQUEST_REPLY_H_ */
