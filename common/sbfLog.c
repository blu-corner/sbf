#include "sbfLog.h"

static const char* gSbfLogLevels[] = { "DEBUG",
                                       "INFO",
                                       "WARN",
                                       "ERROR",
                                       "OFF" };

static void
sbfDefaultLogCb (sbfLogLevel level, const char* message, void* closure)
{
    struct timeval tv;
    struct tm      tm;
    time_t         t;

    gettimeofday (&tv, NULL);
    t = tv.tv_sec;
    gmtime_r (&t, &tm);

    fprintf (stderr,
             "%04u-%02u-%02u %02u:%02u:%02u.%06u %-5s %s"
#ifdef WIN32
             "\r\n",
#else
             "\n",
#endif
             tm.tm_year + 1900,
             tm.tm_mon + 1,
             tm.tm_mday,
             tm.tm_hour,
             tm.tm_min,
             tm.tm_sec,
             (u_int)tv.tv_usec,
             gSbfLogLevels[level],
             message);
#ifdef WIN32
    fflush (stderr);
#endif
}

static sbfLogLevel gSbfLogLevel = SBF_LOG_DEBUG;
static sbfLogCb    gSbfLogCb = sbfDefaultLogCb;
static void*       gSbfLogClosure;

void
sbfLog_setLevel (sbfLogLevel level)
{
    gSbfLogLevel = level;
}

sbfLogLevel
sbfLog_getLevel (const char* s)
{
    size_t slen;
    u_int  i;

    slen = strlen (s);
    for (i = 0; i < sizeof gSbfLogLevels / (sizeof gSbfLogLevels[0]); i++)
    {
        if (strncasecmp (gSbfLogLevels[i], s, slen) == 0)
            return i;
    }
    return gSbfLogLevel;
}

void
sbfLog_setCallback (sbfLogCb cb, void* closure)
{
    if (cb == NULL)
    {
        gSbfLogCb = sbfDefaultLogCb;
        gSbfLogClosure = NULL;
    }
    else
    {
        gSbfLogCb = cb;
        gSbfLogClosure = closure;
    }
}

void
sbfLog_log (sbfLogLevel level, const char* fmt, ...)
{
    char    tmp[BUFSIZ];
    va_list ap;

    if (gSbfLogLevel > level)
        return;

    va_start (ap, fmt);
    vsnprintf (tmp, sizeof tmp, fmt, ap);
    va_end (ap);

    gSbfLogCb (level, tmp, gSbfLogClosure);
}

void
sbfLog_logData (sbfLogLevel level, void* buf, size_t len)
{
    u_char* ptr;
    u_char* end;
    char    line[79];
    char*   out;
    char    text[17];
    int     i;

    if (gSbfLogLevel > level)
        return;

    end = (u_char*)buf + len;
    for (ptr = buf; ptr < end; ptr += 16)
    {
        for (i = 0; i < 16; i++)
        {
            if (ptr + i == end)
                break;
            if (isprint (ptr[i]))
                text[i] = ptr[i];
            else
                text[i] = '.';
        }
        text[i] = '\0';

        out = line + sprintf (line, "%08tx  ", ptr - (u_char*)buf);
        for (i = 0; i < 16; i++)
        {
            if (i == 8)
                out += sprintf (out, " ");
            if (ptr + i >= end)
                out += sprintf (out, "   ");
            else
                out += sprintf (out, "%02hhx ", ptr[i]);
        }
        sprintf (out, " |%s|", text);

        sbfLog_log (level, "%s", line);
    }
}
