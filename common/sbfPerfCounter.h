#ifndef _SBF_PERF_COUNTER_H_
#define _SBF_PERF_COUNTER_H_

#include "sbfCommon.h"

SBF_BEGIN_DECLS

uint64_t sbfPerfCounter_frequency (void); /* per microsecond */

uint64_t sbfPerfCounter_ticks (double microseconds);
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
