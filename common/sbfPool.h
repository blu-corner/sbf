#ifndef _SBF_POOL_H_
#define _SBF_POOL_H_

#include "sbfCommon.h"

SBF_BEGIN_DECLS

typedef struct sbfPoolImpl* sbfPool;

sbfPool sbfPool_create (size_t itemSize);
void sbfPool_destroy (sbfPool pool);

static inline size_t sbfPool_getItemSize (sbfPool pool);

static inline void* sbfPool_get (sbfPool pool);
static inline void* sbfPool_getZero (sbfPool pool);
static inline void sbfPool_put (void* data);

#include "sbfPoolInline.h"

SBF_END_DECLS

#endif /* _SBF_POOL_H_ */
