#include "sbfPerfCounter.h"

#ifdef WIN32
uint64_t
sbfPerfCounter_frequency (void)
{
    LARGE_INTEGER f;

    QueryPerformanceFrequency (&f);
    return f.QuadPart;
}
#else
uint64_t
sbfPerfCounter_frequency (void)
{
    static uint64_t    frequency;
    FILE*              f;
    char*              line;
    size_t             size;
    unsigned long long n;

    if (frequency != 0)
        return frequency;

    f = fopen ("/proc/cpuinfo", "r");
    if (f == NULL)
        sbfFatal ("couldn't open /proc/cpuinfo");

    line = NULL;
    while (getline (&line, &size, f) != -1)
    {
        if (sscanf (line, "cpu MHz         : %llu.", &n) == 1)
        {
            frequency = n * 1000000ULL;
            sbfLog_debug ("got CPU frequency of %llu MHz", n);
            break;
        }
    }
    free (line);

    fclose (f);

    if (frequency == 0)
        sbfFatal ("couldn't read CPU frequency");
    return frequency;
}
#endif

double
sbfPerfCounter_microseconds (uint64_t v)
{
    return (double)v / sbfPerfCounter_frequency ();
}
