/*!
   \file sbfRefCount.h
   \brief This file declares the structures and functions to handle
   the ownership of structure instances.
   \copyright © Copyright 2016 Neueda all rights reserved.
 */

#ifndef _SBF_REFCOUNT_H_
#define _SBF_REFCOUNT_H_

#include "sbfCommon.h"

SBF_BEGIN_DECLS

/*! Declares the reference count's handler. */
typedef struct { uint32_t mRefCount; } sbfRefCount;

/*! Macro returning the reference count. */
#define sbfRefCount_get(r) ((r)->mRefCount)

/*!
   Initialise the reference count to a given number.
   \param r the reference count's handler.
   \param n the initial count.
 */
static SBF_INLINE void
sbfRefCount_init (sbfRefCount* r, u_short n)
{
    r->mRefCount = n;
}

/*!
   Increases the reference count by one.
   \param r the reference count's handler.
 */
static SBF_INLINE void
sbfRefCount_increment (sbfRefCount* r)
{
#ifdef WIN32
    InterlockedIncrement (&r->mRefCount);
#else
    __sync_fetch_and_add (&r->mRefCount, 1);
#endif
}

/*!
   Decreases the reference count by one.
   \param r the reference count's handler.
 */
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
