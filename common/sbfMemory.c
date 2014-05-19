#include "sbfMemory.h"

void*
sbfMemory_malloc (size_t size)
{
    void* ptr;

    if (size == 0)
        SBF_FATAL ("zero size");

    ptr = malloc (size);
    if (ptr == NULL)
        SBF_FATAL ("out of memory");
    return ptr;
}

void*
sbfMemory_calloc (size_t nmemb, size_t size)
{
    void *ptr;

    if (size == 0 || nmemb == 0)
        SBF_FATAL ("zero size");
    if (SIZE_MAX / nmemb < size)
        SBF_FATAL ("size too big");

    ptr = calloc(nmemb, size);
    if (ptr == NULL)
        SBF_FATAL ("out of memory");
    return ptr;
}

void*
sbfMemory_realloc (void* ptr, size_t nmemb, size_t size)
{
    size_t newsize;

    newsize = nmemb * size;
    if (newsize == 0)
        SBF_FATAL ("zero size");
    if (SIZE_MAX / nmemb < size)
        SBF_FATAL ("size too big");

    ptr = realloc (ptr, newsize);
    if (ptr == NULL)
        SBF_FATAL ("out of memory");
    return ptr;
}

char*
sbfMemory_strdup (const char* s)
{
    char* ss;

    ss = strdup (s);
    if (ss == NULL)
        SBF_FATAL ("out of memory");
    return ss;
}

void*
sbfMemory_getGlobal (size_t size)
{
    void* ptr;

#ifdef WIN32
    ptr = LocalAlloc (0, size);
#else
    ptr = malloc (size);
#endif
    if (ptr == NULL)
        SBF_FATAL ("out of memory");
    return ptr;
}

void
sbfMemory_freeGlobal (void* ptr)
{
#ifdef WIN32
    LocalFree (ptr);
#else
    free (ptr);
#endif
}
