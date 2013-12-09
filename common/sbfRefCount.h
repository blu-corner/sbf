#ifndef _SBF_REFCOUNT_H_
#define _SBF_REFCOUNT_H_

#include "sbfCommon.h"

SBF_BEGIN_DECLS

typedef struct { int mRefCount; } sbfRefCount;

#define sbfRefCount_get(r) ((r)->mRefCount)

static inline void
sbfRefCount_init (sbfRefCount* r, u_short n)
{
    r->mRefCount = n;
}

static inline void
sbfRefCount_increment (sbfRefCount* r)
{
    __sync_fetch_and_add (&r->mRefCount, 1);
}

static inline int
sbfRefCount_decrement (sbfRefCount* r)
{
    return __sync_sub_and_fetch (&r->mRefCount, 1) == 0;
}

SBF_END_DECLS

#endif /* _SBF_REFCOUNT_H_ */
