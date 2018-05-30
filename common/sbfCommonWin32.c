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
sbfThread_create (sbfThread* thread, void* (*cb) (void*), void* closure)
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

char*
strsep (char** stringp, const char* delim)
{
    char* start = *stringp;
    char* p;

    p = (start != NULL) ? strpbrk(start, delim) : NULL;

    if (p == NULL)
    {
        *stringp = NULL;
    }
    else
    {
        *p = '\0';
        *stringp = p + 1;
    }

    return start;
}

ssize_t
pwrite (int fd, const void *buf, size_t count, off_t offset)
{
    // TODO
    // get offset
    // write
    // reset offset
    return 0;
}

void*
mmap (void* addr, size_t length, int prot, int flags, int fd, off_t offset)
{
    HANDLE mapping;
    HANDLE handle;
    void*  vp;
    DWORD  type;

    if (addr != NULL || flags != 0 || offset != 0)
        goto fail;
    if (prot != PROT_READ && prot != (PROT_READ|PROT_WRITE))
        goto fail;

    handle = (HANDLE)_get_osfhandle (fd);
    if (handle == INVALID_HANDLE_VALUE)
        return NULL;

    if (prot == (PROT_READ|PROT_WRITE))
        type = PAGE_READWRITE;
    else if (prot == PROT_READ)
        type = PAGE_READONLY;
    mapping = CreateFileMapping (handle, NULL, type, 0, 0, NULL);
    if (mapping == NULL)
        goto fail;

    if (prot == (PROT_READ|PROT_WRITE))
        type = FILE_MAP_ALL_ACCESS;
    else if (prot == PROT_READ)
        type = FILE_MAP_READ;
    vp = MapViewOfFile (mapping, type, 0, 0, length);
    if (vp != NULL)
        goto fail;

    return vp;

fail:
    errno = EINVAL;
    return MAP_FAILED;
}

void
munmap (void* addr, size_t length)
{
    UnmapViewOfFile (addr);
}
