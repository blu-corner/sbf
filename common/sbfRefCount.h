#ifndef _SBF_REFCOUNT_H_
#define _SBF_REFCOUNT_H_

#include "sbfCommon.h"

SBF_BEGIN_DECLS

typedef struct { uint32_t mRefCount; } sbfRefCount;

#define sbfRefCount_get(r) ((r)->mRefCount)

static SBF_INLINE void
sbfRefCount_init (sbfRefCount* r, u_short n)
{
    r->mRefCount = n;
}

static SBF_INLINE void
sbfRefCount_increment (sbfRefCount* r)
{
#ifdef WIN32
    InterlockedIncrement (&r->mRefCount);
#else
    __sync_fetch_and_add (&r->mRefCount, 1);
#endif
}

static SBF_INLINE int
sbfRefCount_decrement (sbfRefCount* r)
{
#ifdef WIN32
    return InterlockedDecrement (&r->mRefCount) == 0;
#else
    return __sync_sub_and_fetch (&r->mRefCount, 1) == 0;
#endif
}

SBF_END_DECLS

#endif /* _SBF_REFCOUNT_H_ */
