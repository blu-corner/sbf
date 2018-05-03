#ifndef _SBF_COMMON_WIN32_H_
#define _SBF_COMMON_WIN32_H_

#ifndef _CRT_NONSTDC_NO_WARNINGS
#define _CRT_NONSTDC_NO_WARNINGS
#endif
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
#include <wchar.h>

#include <process.h>
#include <winsock2.h> /* must be before windows.h */
#include <io.h>
#include <iphlpapi.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <getopt.h>

SBF_BEGIN_DECLS

typedef intptr_t sbfSocket;
typedef SSIZE_T ssize_t;

#ifndef PATH_MAX
#define PATH_MAX MAX_PATH
#endif
#ifndef __func__
#define __func__ __FUNCTION__
#endif

#define SBF_EOL "\r\n"
#define SBF_SHLIB_SUFFIX /* not used */

#define SBF_PACKED(d) \
    __pragma(pack(push, 1)) \
    d \
    __pragma(pack(pop))
#define SBF_PRINTFLIKE(a, b)
#define SBF_UNUSED
#define SBF_INLINE _forceinline
#define SBF_PURE
#define SBF_CONST
#define SBF_DEAD
#define SBF_TLS __declspec (thread)

#define SBF_LIKELY(e) e
#define SBF_UNLIKELY(e) e

#define SBF_ASSERT(x) assert (x)

typedef HANDLE sbfThread;
int sbfThread_create (sbfThread* thread, void* (*cb) (void*), void* closure);
#define sbfThread_join(t) WaitForSingleObject (t, INFINITE)

typedef CONDITION_VARIABLE sbfCondVar;
#define sbfCondVar_init(c) InitializeConditionVariable (c)
#define sbfCondVar_init_attr(c, a) InitializeConditionVariable (c)
#define sbfCondVar_destroy(c)
#define sbfCondVar_wait(c, m) SleepConditionVariableCS (c, m, INFINITE)
#define sbfCondVar_broadcast(c) WakeAllConditionVariable (c)
#define sbfCondVar_signal(c) WakeConditionVariable (c)

typedef void sbfCondAttr;
#define sbfCondAttr_init(a)
#define sbfCondAttr_setpshared(a, p)

typedef void sbfMutexAttr;
#define sbfMutexAttr_init(a)
#define sbfMutexAttr_setpshared(a, p)

typedef CRITICAL_SECTION sbfMutex;
#define sbfMutex_init(m, recursive) InitializeCriticalSection (m)
#define sbfMutex_init_attr(m, a) InitializeCriticalSection (m)
#define sbfMutex_destroy(m) DeleteCriticalSection (m)
#define sbfMutex_lock(m) EnterCriticalSection (m)
#define sbfMutex_unlock(m) LeaveCriticalSection (m)

#define sbfSpinLock sbfMutex
#define sbfSpinLock_init(l) sbfMutex_init (l, 0)
#define sbfSpinLock_destroy sbfMutex_destroy
#define sbfSpinLock_lock sbfMutex_lock
#define sbfSpinLock_unlock sbfMutex_unlock

static SBF_INLINE void
sleep (u_int seconds)
{
    Sleep (seconds * 1000U);
}

static SBF_INLINE char*
basename (char* path)
{
    char name[_MAX_FNAME];
    _splitpath (path, NULL, NULL, name, NULL);
    memcpy (path, name, strlen (name) + 1);
    return path;
}

#define snprintf _snprintf
#define strcasecmp _stricmp
#define strncasecmp _strnicmp
#define strtoll _strtoi64
#define strtoull _strtoui64
#define vsnprintf _vsnprintf
#define fseeko _fseeki64
#define stat _stat
#define fstat _fstat

char* fgetln(FILE* fp, size_t* len);
char* strsep(char** stringp, const char* delim);
size_t strlcat (char *dst, const char *src, size_t siz);
size_t strlcpy (char *dst, const char *src, size_t siz);
ssize_t pwrite(int fd, const void *buf, size_t count, off_t offset);

int gettimeofday (struct timeval* tv, struct timezone* tz);
#define gmtime_r(tp, tm) gmtime_s (tm, tp)
#define localtime_r(tp, tm) localtime_s (tm, tp)

int asprintf (char** ret, const char* fmt, ...);
int vasprintf (char** ret, const char* fmt, va_list ap);

typedef enum
{
    CLOCK_REALTIME,
} clockid_t;
static SBF_INLINE int
clock_gettime (clockid_t clock_id, struct timespec* tp)
{
    struct timeval tv;

    if (clock_id != CLOCK_REALTIME)
    {
        errno = EINVAL;
        return -1;
    }

    if (gettimeofday (&tv, NULL) != 0)
        return -1;
    tp->tv_sec = tv.tv_sec;
    tp->tv_nsec = tv.tv_usec * 1000U;

    return 0;
}

#define O_RDWR _O_RDWR
#define O_CREAT _O_CREAT
// https://github.com/nodejs/node/issues/2182
#define O_NOATIME 0x40000

#define PROT_READ 2
#define PROT_WRITE 1
#define MAP_ANON 1
#define MAP_PRIVATE 0
#define MAP_SHARED 0
#define MAP_POPULATE 0

#define MAP_FAILED ((void*)-1)

void* mmap (void* addr,
            size_t length,
            int prot,
            int flags,
            int fd,
            off_t offset);
void munmap (void* addr, size_t length);

SBF_END_DECLS

#endif /* _SBF_COMMON_WIN32_H_ */
