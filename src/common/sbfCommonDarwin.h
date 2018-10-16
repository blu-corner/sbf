#ifndef _SBF_COMMON_DARWIN_H_
#define _SBF_COMMON_DARWIN_H_

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/syscall.h>

#include <arpa/inet.h>

#include <netinet/in.h>
#include <netinet/tcp.h>

#include <assert.h>
#include <ctype.h>
#include <dlfcn.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <fnmatch.h>
#include <glob.h>
#include <ifaddrs.h>
#include <libgen.h>
#include <limits.h>
#include <math.h>
#include <netdb.h>
#include <pthread.h>
#include <regex.h>
#include <signal.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <os/lock.h>
#include <libkern/OSAtomic.h>

SBF_BEGIN_DECLS

#define __thread /* XXX */

#define O_NOATIME 0
#define MAP_POPULATE 0
#define HOST_NAME_MAX 255

typedef int sbfSocket;

#define SBF_EOL "\n"
#define SBF_SHLIB_SUFFIX ".dylib"

#define SBF_PACKED(d) d __attribute__ ((__packed__))
#define SBF_PRINTFLIKE(a, b) __attribute__ ((format (printf, a, b)))
#define SBF_UNUSED __attribute__ ((__unused__))
#define SBF_INLINE __inline
#define SBF_PURE __attribute__ ((pure))
#define SBF_CONST __attribute__ ((const))
#define SBF_DEAD __attribute__ ((noreturn))
#define SBF_TLS __thread

#define SBF_LIKELY(e) __builtin_expect (!!(e), 1)
#define SBF_UNLIKELY(e) __builtin_expect (!!(e), 0)

#define SBF_ASSERT(x) assert (x)

#define SBF_DLLEXPORT

typedef pthread_t sbfThread;
#define sbfThread_create(t, cb, closure) pthread_create (t, NULL, cb, closure)
#define sbfThread_join(t) pthread_join (t, NULL)

typedef pthread_cond_t sbfCondVar;
#define sbfCondVar_init(c) pthread_cond_init (c, NULL)
#define sbfCondVar_init_attr(c, a) pthread_cond_init (c, a)
#define sbfCondVar_destroy(c) pthread_cond_destroy (c)
#define sbfCondVar_wait(c, m) pthread_cond_wait (c, m)
#define sbfCondVar_broadcast(c) pthread_cond_broadcast (c)
#define sbfCondVar_signal(c) pthread_cond_signal (c)

typedef pthread_condattr_t sbfCondAttr;
#define sbfCondAttr_init(a) pthread_condattr_init (a)
#define sbfCondAttr_setpshared(a, p) pthread_condattr_setpshared (a, p)

typedef pthread_mutexattr_t sbfMutexAttr;
#define sbfMutexAttr_init(a) pthread_mutexattr_init (a)
#define sbfMutexAttr_setpshared(a, p) pthread_mutexattr_setpshared (a, p)

typedef pthread_mutex_t sbfMutex;
int sbfMutex_init (pthread_mutex_t* m, int recursive);
#define sbfMutex_init_attr(m, a) pthread_mutex_init (m, a)
#define sbfMutex_destroy(m) pthread_mutex_destroy (m)
#define sbfMutex_lock(m) pthread_mutex_lock (m)
#define sbfMutex_unlock(m) pthread_mutex_unlock (m)

typedef os_unfair_lock sbfSpinLock;
#define sbfSpinLock_init(s) memset(s, 0, sizeof(sbfSpinLock))
#define sbfSpinLock_destroy(s)
#define sbfSpinLock_lock(s) os_unfair_lock_lock (s)
#define sbfSpinLock_unlock(s) os_unfair_lock_unlock (s)

SBF_END_DECLS

#endif /* _SBF_COMMON_DARWIN_H_ */

