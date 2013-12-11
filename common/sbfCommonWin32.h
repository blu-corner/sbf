#ifndef _SBF_COMMON_WIN32_H_
#define _SBF_COMMON_WIN32_H_

#define _CRT_SECURE_NO_WARNINGS

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <winsock2.h> /* must be before windows.h */
#include <windows.h>

SBF_BEGIN_DECLS

typedef intptr_t sbfSocket;
void sbfCloseSocket (sbfSocket s);

#ifndef __func__
#define __func__ __FUNCTION__
#endif

#define SBF_PACKED(d)		\
    __pragma(pack(push, 1)) \
    d                       \
    __pragma(pack(pop))
#define SBF_PRINTFLIKE(a, b)
#define SBF_UNUSED
#define SBF_INLINE _forceinline

#define SBF_LIKELY(e)
#define SBF_UNLIKELY(e)

typedef HANDLE pthread_t;
int pthread_create (pthread_t* thread,
                    void* unused,
                    void* (*cb) (void*),
                    void* closure);

typedef CRITICAL_SECTION pthread_mutex_t;
#define pthread_mutex_init(m, a) InitializeCriticalSection (m)
#define pthread_mutex_destroy(m) DeleteCriticalSection (m)
#define pthread_mutex_lock(m) EnterCriticalSection (m)
#define pthread_mutex_unlock(m) LeaveCriticalSection (m)

#define pthread_spinlock_t pthread_mutex_t
#define pthread_spin_init pthread_mutex_init
#define pthread_spin_destroy pthread_mutex_destroy
#define pthread_spin_lock pthread_mutex_lock
#define pthread_spin_unlock pthread_mutex_unlock

#define snprintf _snprintf
#define strcasecmp _stricmp
#define strdup _strdup
#define strncasecmp _strnicmp
#define vsnprintf _vsnprintf

int gettimeofday (struct timeval* tv, struct timezone* tz);
#define gmtime_r(tp, tm) gmtime_s (tm, tp)

SBF_END_DECLS

#endif /* _SBF_COMMON_WIN32_H_ */
