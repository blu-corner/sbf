#ifndef _SBF_SUB_H_
#define _SBF_SUB_H_

#include "sbfBuffer.h"
#include "sbfCommon.h"
#include "sbfQueue.h"

SBF_BEGIN_DECLS

struct sbfTportImpl;
struct sbfTopicImpl;

typedef struct sbfSubImpl* sbfSub;

typedef void (*sbfSubReadyCb) (sbfSub sub, void* closure);

typedef void (*sbfSubMessageCb) (sbfSub sub, sbfBuffer buffer, void* closure);

sbfSub sbfSub_create (struct sbfTportImpl* tport,
                      sbfQueue queue,
                      const char* topic,
                      sbfSubReadyCb readyCb,
                      sbfSubMessageCb messageCb,
                      void* closure);

void sbfSub_destroy (sbfSub sub);

struct sbfTopicImpl* sbfSub_getTopic (sbfSub sub);

SBF_END_DECLS

#endif /* _SBF_SUB_H_ */
