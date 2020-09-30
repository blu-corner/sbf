#include "SbfLog.hpp"

neueda::SbfLog::SbfLog ()
{
    mValue = sbfLog_create (NULL, "%s", "");
}

neueda::SbfLog::~SbfLog ()
{
    if (getHandle () != NULL)
        sbfLog_destroy (getHandle ());
}

void neueda::SbfLog::log (sbfLogLevel level, const char* fmt, ...)
{
    if (getHandle () == NULL)
        return;

    if (getLevel () > level)
        return;

    va_list ap;
    va_start (ap, fmt);
    sbfLog_vlog (getHandle (), level, fmt, ap);
    va_end (ap);
}

void neueda::SbfLog::logData (sbfLogLevel level, void* buf, size_t len)
{
    if (getHandle () == NULL)
        return;

    sbfLog_logData (getHandle (), level, buf, len);
}

sbfLog neueda::SbfLog::getHandle ()
{
    return mValue;
}

sbfLogLevel neueda::SbfLog::getLevel()
{
    if (getHandle () != NULL)
    {
        return sbfLog_getLevel (getHandle ());
    }
    else
    {
        return SBF_LOG_INFO;
    }
}

void neueda::SbfLog::setLevel(sbfLogLevel level)
{
    if (getHandle () != NULL)
        sbfLog_setLevel (getHandle (), level);
}

bool neueda::SbfLog::setLevelFromString (const char* s)
{
    bool ret = false;
    int* found = new int(0);
    sbfLogLevel level = sbfLog_levelFromString (s, found);
    fflush (stdout);
    if (found)
    {
        setLevel(level);
        ret = true;
    }
    delete found;
    return ret;
}

const char* neueda::SbfLog::levelToString (sbfLogLevel level)
{
    return sbfLog_levelToString (level);
}
