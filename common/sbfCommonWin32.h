#ifndef _SBF_COMMON_WIN32_H_
#define _SBF_COMMON_WIN32_H_

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <assert.h>
#include <errno.h>
#include <regex.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <winsock2.h> /* must be before windows.h */
#include <iphlpapi.h>
#include <ws2tcpip.h>
#include <windows.h>

SBF_BEGIN_DECLS

typedef intptr_t sbfSocket;

#ifndef PATH_MAX
#define PATH_MAX MAX_PATH
#endif
#ifndef __func__
#define __func__ __FUNCTION__
#endif

#define SBF_EOL "\r\n"

#define SBF_PACKED(d) \
    __pragma(pack(push, 1)) \
    d \
    __pragma(pack(pop))
#define SBF_PRINTFLIKE(a, b)
#define SBF_UNUSED
#define SBF_INLINE _forceinline

#define SBF_LIKELY(e)
#define SBF_UNLIKELY(e)

#define SBF_ASSERT(x) assert (x)

#define SBF_DLLEXPORT __declspec (dllexport)

typedef HANDLE sbfThread;
int sbfThread_create (sbfThread* thread, void* (*cb) (void*), void* closure);
#define sbfThread_join(t) WaitForSingleObject (t, INFINITE)

typedef CONDITION_VARIABLE sbfCondVar;
#define sbfCondVar_init(c) InitializeConditionVariable (c)
#define sbfCondVar_destroy(c)
#define sbfCondVar_wait(c, m) SleepConditionVariableCS (c, m, INFINITE)
#define sbfCondVar_broadcast(c) WakeAllConditionVariable (c)
#define sbfCondVar_signal(c) WakeConditionVariable (c)

typedef CRITICAL_SECTION sbfMutex;
#define sbfMutex_init(m, recursive) InitializeCriticalSection (m)
#define sbfMutex_destroy(m) DeleteCriticalSection (m)
#define sbfMutex_lock(m) EnterCriticalSection (m)
#define sbfMutex_unlock(m) LeaveCriticalSection (m)

#define sbfSpinLock sbfMutex
#define sbfSpinLock_init(l) sbfMutex_init (l, 0)
#define sbfSpinLock_destroy sbfMutex_destroy
#define sbfSpinLock_lock sbfMutex_lock
#define sbfSpinLock_unlock sbfMutex_unlock

#define fileno _fileno
#define getpid GetCurrentProcessId
#define sleep(seconds) Sleep (seconds * 1000U)
#define snprintf _snprintf
#define strcasecmp _stricmp
#define strdup _strdup
#define strncasecmp _strnicmp
#define strtoll _strtoi64
#define strtoull _strtoui64
#define vsnprintf _vsnprintf

int gettimeofday (struct timeval* tv, struct timezone* tz);
#define gmtime_r(tp, tm) gmtime_s (tm, tp)

int asprintf (char** ret, const char* fmt, ...);
int vasprintf (char** ret, const char* fmt, va_list ap);

extern int   BSDopterr;
extern int   BSDoptind;
extern int   BSDoptopt;
extern int   BSDoptreset;
extern char* BSDoptarg;
int     BSDgetopt(int, char* const*, const char*);
#define getopt(ac, av, o) BSDgetopt (ac, av, o)
#define opterr            BSDopterr
#define optind            BSDoptind
#define optopt            BSDoptopt
#define optreset          BSDoptreset
#define optarg            BSDoptarg

SBF_END_DECLS

#endif /* _SBF_COMMON_WIN32_H_ */
