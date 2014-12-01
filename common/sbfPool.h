#ifndef _SBF_POOL_H_
#define _SBF_POOL_H_

#include "sbfCommon.h"
#include "sbfAtomic.h"

SBF_BEGIN_DECLS

typedef struct sbfPoolImpl* sbfPool;

void sbfPool_init (sbfLog log);
sbfPool sbfPool_default (size_t size);

#define sbfPool_create(itemSize) \
    sbfPool_create1 (itemSize, __FUNCTION__, __LINE__)
sbfPool sbfPool_create1 (size_t itemSize, const char* function, u_int line);
void sbfPool_destroy (sbfPool pool);

void sbfPool_registerAtExit (sbfPool pool);
void sbfPool_showSummary (sbfLog log, sbfLogLevel level);

static SBF_INLINE size_t sbfPool_getItemSize (sbfPool pool);

static SBF_INLINE void* sbfPool_get (sbfPool pool);
static SBF_INLINE void* sbfPool_getZero (sbfPool pool);
static SBF_INLINE void sbfPool_put (void* data);

#include "sbfPoolInline.h"

SBF_END_DECLS

#endif /* _SBF_POOL_H_ */
