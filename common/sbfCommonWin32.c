#include "sbfCommon.h"

#include <evutil.h>

typedef struct
{
    void* (*mCb) (void*);
    void* mClosure;
} sbfCommonThreadClosure;

static DWORD __stdcall
sbfCommonThreadEntry (LPVOID closure)
{
    sbfCommonThreadClosure* tc = closure;

    tc->mCb (tc->mClosure);
    free (tc);

    return 0;
}

int
gettimeofday (struct timeval* tv, struct timezone* tz)
{
    return evutil_gettimeofday (tv, tz);
}

int
pthread_create (pthread_t* thread,
                void* unused,
                void* (*cb) (void*),
                void* closure)
{
    HANDLE                  h;
    sbfCommonThreadClosure* tc;

    tc = xmalloc (sizeof *tc);
    tc->mCb = cb;
    tc->mClosure = closure;

    h = CreateThread (NULL, 0, sbfCommonThreadEntry, tc, 0, NULL);
    if (h == NULL)
    {
        free (tc);
        errno = EIO;
        return -1;
    }

    *thread = h;
    return 0;
}

int
asprintf (char** ret, const char* fmt, ...)
{
    va_list ap;
    int     n;

    va_start (ap, fmt);
    n = vasprintf (ret, fmt, ap);
    va_end (ap);

    return (n);
}

int
vasprintf (char** ret, const char* fmt, va_list ap)
{
    int      n;
    va_list  ap2;

    ap2 = ap; /* va_copy (ap2, ap); */

    if ((n = vsnprintf (NULL, 0, fmt, ap)) < 0)
        goto error;

    *ret = xmalloc (n + 1);
    if ((n = vsnprintf (*ret, n + 1, fmt, ap2)) < 0) {
        free (*ret);
        goto error;
    }

    return (n);

error:
    *ret = NULL;
    return (-1);
}
