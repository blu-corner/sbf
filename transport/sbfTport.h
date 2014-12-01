#ifndef _SBF_TPORT_H_
#define _SBF_TPORT_H_

#include "sbfBatch.h"
#include "sbfCommon.h"
#include "sbfMw.h"
#include "sbfPub.h"
#include "sbfSub.h"
#include "sbfTopic.h"

SBF_BEGIN_DECLS

typedef struct sbfTportImpl* sbfTport;

sbfTport sbfTport_create (sbfMw mw, const char* name, uint64_t mask);
void sbfTport_destroy (sbfTport tport);

const char* sbfTport_getName (sbfTport tport);
sbfMw sbfTport_getMw (sbfTport tport);

SBF_END_DECLS

#endif /* _SBF_TPORT_H_ */
