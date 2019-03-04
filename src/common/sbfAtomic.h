/*!
   \file sbfAtomic.h
   \brief This file defines the helper structures and functions to handle atomics.
   \Copyright 2014-2018 Neueda Ltd.
 */

#ifndef _SBF_ATOMIC_H_
#define _SBF_ATOMIC_H_

#include "sbfCommon.h"

static SBF_INLINE int64_t
sbfAtomic_compareAndSwap (volatile int64_t* ptr,
                          int64_t compare,
                          int64_t replace)
{
#ifdef _WIN32
    return _InterlockedCompareExchange64 (ptr, replace, compare) == compare;
#else
    return __sync_bool_compare_and_swap (ptr, compare, replace);
#endif
}

static SBF_INLINE int64_t
sbfAtomic_swap (volatile int64_t* ptr, int64_t value)
{
#ifdef _WIN32
    return _InterlockedExchange64 (ptr, value);
#else
    return __sync_lock_test_and_set (ptr, value);
#endif
}

static SBF_INLINE int64_t
sbfAtomic_increment (volatile int64_t* ptr)
{
#ifdef _WIN32
    return _InterlockedIncrement64 (ptr);
#else
    return __sync_fetch_and_add (ptr, 1);
#endif
}

static SBF_INLINE int64_t
sbfAtomic_decrement (volatile int64_t* ptr)
{
#ifdef _WIN32
    return _InterlockedDecrement64 (ptr);
#else
    return __sync_sub_and_fetch (ptr, 1);
#endif
}

#endif /* _SBF_ATOMIC_H_ */
