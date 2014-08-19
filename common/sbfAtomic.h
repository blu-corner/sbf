#ifndef _SBF_ATOMIC_H_
#define _SBF_ATOMIC_H_

#include "sbfCommon.h"

#ifdef WIN32
#define sbfAtomic_swap InterlockedExchange
#define sbfAtomic_swapP InterlockedExchangePointer
#else
#define sbfAtomic_swap __sync_lock_test_and_set
#define sbfAtomic_swapP __sync_lock_test_and_set
#endif

#ifdef WIN32
#define sbfAtomic_compareAndSwap InterlockedCompareExchange
#define sbfAtomic_compareAndSwapP InterlockedCompareExchangePointer
#else
#define sbfAtomic_compareAndSwap __sync_bool_compare_and_swap
#define sbfAtomic_compareAndSwapP __sync_bool_compare_and_swap
#endif

#endif /* _SBF_ATOMIC_H_ */
