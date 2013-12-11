#include "sbfCpuFreq.h"

static uint64_t gSbfCpuFreq;

#ifdef WIN32
uint64_t
sbfCpuFreq_get (void)
{
    LARGE_INTEGER f;

    QueryPerformanceFrequency (&f);
    return f.QuadPart;
}
#else
uint64_t
sbfCpuFreq_get (void)
{
    FILE*              f;
    char*              line;
    size_t             size;
    unsigned long long n;

    if (gSbfCpuFreq != 0)
        return gSbfCpuFreq;

    f = fopen ("/proc/cpuinfo", "r");
    if (f == NULL)
        sbfFatal ("couldn't open /proc/cpuinfo");

    line = NULL;
    while (getline (&line, &size, f) != -1)
    {
        if (sscanf (line, "cpu MHz         : %llu.", &n) == 1)
        {
            gSbfCpuFreq = n;
            sbfLog_debug ("got CPU frequency of %llu MHz",
                          (unsigned long long)gSbfCpuFreq);
            break;
        }
    }
    free (line);

    fclose (f);

    if (gSbfCpuFreq == 0)
        sbfFatal ("couldn't read CPU frequency");
    return gSbfCpuFreq;
}
#endif
