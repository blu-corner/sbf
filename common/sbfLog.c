#include "sbfLog.h"
#include "sbfLogPrivate.h"

static const char* gSbfLogLevels[] = { "DEBUG",
                                       "INFO",
                                       "WARN",
                                       "ERROR",
                                       "OFF" };

sbfLog
sbfLog_create (sbfLogFile lf, const char* fmt, ...)
{
    sbfLog  log;
    va_list ap;
    char*   tmp;

    log = xcalloc (1, sizeof *log);
    log->mFile = lf;
    log->mLevel = SBF_LOG_INFO;

    va_start (ap, fmt);
    xvasprintf (&tmp, fmt, ap);
    va_end (ap);

    if (*tmp != '\0')
        xasprintf (&log->mPrefix, "(%s) ", tmp);
    else
        log->mPrefix = xstrdup ("");
    free (tmp);

    return log;
}

void
sbfLog_destroy (sbfLog log)
{
    free (log->mPrefix);

    free (log);
}

sbfLogLevel
sbfLog_getLevel (sbfLog log)
{
    return log->mLevel;
}

void
sbfLog_setLevel (sbfLog log, sbfLogLevel level)
{
    log->mLevel = level;
}

sbfLogLevel
sbfLog_levelFromString (const char* s, int* found)
{
    size_t slen;
    u_int  i;

    slen = strlen (s);
    for (i = 0; i < SBF_ARRAYSIZE (gSbfLogLevels); i++)
    {
        if (strncasecmp (gSbfLogLevels[i], s, slen) == 0)
        {
            if (found != NULL)
                *found = 1;
            return i;
        }
    }
    if (found != NULL)
        *found = 0;
    return SBF_LOG_OFF;
}

const char*
sbfLog_levelToString (sbfLogLevel level)
{
    if ((u_int)level >= SBF_ARRAYSIZE (gSbfLogLevels))
        return "UNKNOWN";
    return gSbfLogLevels[(u_int)level];
}

void
sbfLog_log (sbfLog log, sbfLogLevel level, const char* fmt, ...)
{
    va_list ap;

    if (log->mLevel > level)
        return;

    va_start (ap, fmt);
    sbfLog_vlog (log, level, fmt, ap);
    va_end (ap);
}

void
sbfLog_vlog (sbfLog log, sbfLogLevel level, const char* fmt, va_list ap)
{
    struct timeval  tv;
    struct tm       tm;
    time_t          t;
    char            message[512];
    sbfLogFileEntry lfe;

    if (log->mLevel > level)
        return;

    if (log->mFile != NULL)
    {
        lfe = sbfLogFile_get (log->mFile);

        gettimeofday (&lfe->mTime, NULL);
        lfe->mLevel = level;
        strlcpy (lfe->mPrefix, log->mPrefix, sizeof lfe->mPrefix);
        vsnprintf (lfe->mMessage, sizeof lfe->mMessage, fmt, ap);

        sbfLogFile_flush (log->mFile, lfe);
        return;
    }

    gettimeofday (&tv, NULL);
    t = tv.tv_sec;
    localtime_r (&t, &tm);

    vsnprintf (message, sizeof message, fmt, ap);

    fprintf (stderr,
             "%04u-%02u-%02u %02u:%02u:%02u.%06u %-5s %s%s\n",
             tm.tm_year + 1900,
             tm.tm_mon + 1,
             tm.tm_mday,
             tm.tm_hour,
             tm.tm_min,
             tm.tm_sec,
             (u_int)tv.tv_usec,
             gSbfLogLevels[level],
             log->mPrefix,
             message);
#ifdef WIN32
    fflush (stderr);
#endif

    if (log->mHookCb != NULL && level >= log->mHookLevel && !log->mHookInside)
    {
        log->mHookInside = 1;
        log->mHookCb (log, level, message, log->mHookClosure);
        log->mHookInside = 0;
    }
}

void
sbfLog_logData (sbfLog log, sbfLogLevel level, void* buf, size_t len)
{
    u_char* ptr;
    u_char* end;
    char    line[79];
    char*   out;
    char    text[17];
    int     i;

    if (log->mLevel > level)
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

        sbfLog_log (log, level, "%s", line);
    }
}

void
sbfLog_setHook (sbfLog log, sbfLogLevel level, sbfLogHookCb cb, void* closure)
{
    log->mHookLevel = level;
    log->mHookCb = cb;
    log->mHookClosure = closure;
}
