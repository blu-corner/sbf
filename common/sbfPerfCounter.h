/*!
   \file sbfPerfCounter.h
   \brief This file declares the structures and functions to handle
   high performance counters.
   \copyright © Copyright 2016 Neueda all rights reserved.
 */

#ifndef _SBF_PERF_COUNTER_H_
#define _SBF_PERF_COUNTER_H_

#include "sbfCommon.h"

SBF_BEGIN_DECLS

/*!
 \brief Returns the cpu frequency in microsecond.
 If there is an error during the frequency detection zero will be returned.
 \return a positive value indicating the cpu frequency per microsecond or zero
 in case of not being able to determine the frequency.
 */
uint64_t sbfPerfCounter_frequency (void); /* per microsecond */

/*!
   \brief Returns the number of ticks for the given microseconds.
   \param microseconds the number of microseconds.
   \return the number of ticks for the given microseconds.
 */
uint64_t sbfPerfCounter_ticks (double microseconds);

/*!
   \brief Returns the number of microseconds corresponding to the
   given number of CPU ticks.
   \param ticks the number of CPU ticks.
   \return the number of microseconds corresponding to the
   given number of CPU ticks.
 */
double sbfPerfCounter_microseconds (uint64_t ticks);

#ifdef WIN32
static SBF_INLINE uint64_t
sbfPerfCounter_get (void)
{
    LARGE_INTEGER v;

    QueryPerformanceCounter (&v);
    return v.QuadPart;
}
#else
/*!
   \brief Returns the value stored at the high performance counter.
   \return the value stored at the high performance counter.
 */
static SBF_INLINE uint64_t
sbfPerfCounter_get (void)
{
    uint32_t lo;
    uint32_t hi;

    __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
    return (uint64_t)hi << 32 | lo;
}
#endif

SBF_END_DECLS

#endif /* _SBF_PERF_COUNTER_H_ */
