#include "sbfPerfCounter.h"

#ifdef WIN32
uint64_t
sbfPerfCounter_frequency (void)
{
    LARGE_INTEGER f;

    QueryPerformanceFrequency (&f);
    return f.QuadPart / 1000000;
}
#else
#ifdef __APPLE__
uint64_t
sbfPerfCounter_frequency (void)
{
    static uint64_t frequency;
    int64_t         n;
    size_t          nlen = sizeof n;

    if (frequency > 0)
        return frequency;

    if (sysctlbyname("hw.cpufrequency", &n, &nlen, NULL, 0) != 0)
        SBF_FATAL ("couldn't read CPU frequency");

    frequency = n / 1000000;
    return frequency;
}
#else
uint64_t
sbfPerfCounter_frequency (void)
{
    static uint64_t    frequency;
    static int         processed = 0;
    FILE*              f;
    char*              line;
    size_t             size;
    unsigned long long n;

    // Get the frequence and cache it
    if (processed == 0)
    {
        f = fopen ("/proc/cpuinfo", "r");
        // Not supported method to collect CPU Frequency
        if (f == NULL)
            return frequency;

        // Collect CPU frequency looking for cpu MHz line
        line = NULL;
        while (getline (&line, &size, f) != -1)
        {
            if (sscanf (line, "cpu MHz         : %llu.", &n) == 1)
            {
                frequency = n;
                break;
            }
        }

        free (line);
        fclose (f);

        processed = 1;
    }

    return frequency;
}
#endif
#endif

uint64_t
sbfPerfCounter_ticks (double microseconds)
{
    uint64_t frequency;

    frequency = sbfPerfCounter_frequency ();
    return microseconds * (double)frequency;
}

double
sbfPerfCounter_microseconds (uint64_t ticks)
{
    uint64_t frequency;

    frequency = sbfPerfCounter_frequency ();
    return ticks / (double)frequency;
}
