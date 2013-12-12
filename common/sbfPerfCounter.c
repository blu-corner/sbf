#include "sbfPerfCounter.h"

#ifdef WIN32
double
sbfPerfCounter_frequency (void)
{
    LARGE_INTEGER f;

    QueryPerformanceFrequency (&f);
    return f.QuadPart / 1000000.0;
}
#else
double
sbfPerfCounter_frequency (void)
{
    static double      frequency;
    FILE*              f;
    char*              line;
    size_t             size;
    unsigned long long n;

    if (frequency > 0)
        return frequency;

    f = fopen ("/proc/cpuinfo", "r");
    if (f == NULL)
        sbfFatal ("couldn't open /proc/cpuinfo");

    line = NULL;
    while (getline (&line, &size, f) != -1)
    {
        if (sscanf (line, "cpu MHz         : %llu.", &n) == 1)
        {
            sbfLog_debug ("got CPU frequency of %llu MHz", n);
            frequency = n;
            break;
        }
    }
    free (line);

    fclose (f);

    if (!(frequency > 0))
        sbfFatal ("couldn't read CPU frequency");
    return frequency;
}
#endif

double
sbfPerfCounter_ticks (uint64_t microseconds)
{
    return microseconds * sbfPerfCounter_frequency ();
}

double
sbfPerfCounter_microseconds (uint64_t ticks)
{
    return ticks / sbfPerfCounter_frequency ();
}
