#ifndef _SBF_TPORT_H_
#define _SBF_TPORT_H_

#include "sbfCommon.h"
#include "sbfKeyValue.h"
#include "sbfMw.h"
#include "sbfPub.h"
#include "sbfSub.h"
#include "sbfTopic.h"

SBF_BEGIN_DECLS

typedef struct sbfTportImpl* sbfTport;

sbfTport sbfTport_create (sbfMw mw,
                          uint64_t mask,
                          const char* type,
                          sbfKeyValue properties);
void sbfTport_destroy (sbfTport tport);
SBF_END_DECLS

#endif /* _SBF_TPORT_H_ */
