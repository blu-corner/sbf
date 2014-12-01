#ifndef _SBF_COMMON_HANDLER_H_
#define _SBF_COMMON_HANDLER_H_

#include "sbfInterface.h"
#include "sbfMw.h"
#include "sbfTport.h"

SBF_BEGIN_DECLS

typedef struct sbfMcastTopicResolverImpl* sbfMcastTopicResolver;

sbfMcastTopicResolver sbfMcastTopicResolver_create (const char* address,
                                                    sbfLog log);
void sbfMcastTopicResolver_destroy (sbfMcastTopicResolver mtr);

uint32_t sbfMcastTopicResolver_makeAddress (sbfMcastTopicResolver mtr,
                                            sbfTopic topic);

SBF_END_DECLS

#endif /* _SBF_COMMON_HANDLER_H_ */
