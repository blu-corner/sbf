#ifndef _SBF_COMMON_LINUX_H_
#define _SBF_COMMON_LINUX_H_

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>

#include <arpa/inet.h>

#include <netinet/in.h>

#include <assert.h>
#include <ctype.h>
#include <dlfcn.h>
#include <errno.h>
#include <ifaddrs.h>
#include <limits.h>
#include <pthread.h>
#include <regex.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#ifdef  __cplusplus
#define SBF_BEGIN_DECLS extern "C" {
#define SBF_END_DECLS }
#else
#define SBF_BEGIN_DECLS
#define SBF_END_DECLS
#endif

SBF_BEGIN_DECLS

#define SBF_PACKED(d) d __attribute__ ((__packed__))
#define SBF_PRINTFLIKE(a, b) __attribute__ ((format (printf, a, b)))

#define SBF_LIKELY(e) __builtin_expect (!!(e), 1)
#define SBF_UNLIKELY(e) __builtin_expect (!!(e), 0)

#define SBF_MIN(a, b) ((a) > (b) ? (b) : (a))
#define SBF_MAX(a, b) ((a) < (b) ? (b) : (a))

static inline uint64_t
sbfRdtsc (void)
{
    uint32_t lo;
    uint32_t hi;

    __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
    return (uint64_t)hi << 32 | lo;
}

SBF_END_DECLS

#endif /* _SBF_COMMON_LINUX_H_ */

