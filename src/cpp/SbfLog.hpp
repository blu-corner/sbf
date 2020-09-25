#pragma once

#include "sbfLog.h"

namespace neueda
{

class SbfLog {
public:
    /*
     * Restricting the CPP log constructors from printing an initial
     * message.
     */
    SbfLog ()
    {
        mValue = sbfLog_create (NULL, "", "");
    }

    /*
     * TODO constructor for constructing a log file.
     **/

    virtual ~SbfLog ()
    {
        if (getHandle () != NULL)
            sbfLog_destroy (getHandle ());
    }

    virtual void log (sbfLogLevel level, const char* fmt, ...)
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

    virtual void logData (sbfLogLevel level, void* buf, size_t len)
    {
        if (getHandle () == NULL)
            return;

        sbfLog_logData (getHandle (), level, buf, len);
    }

    virtual sbfLog getHandle ()
    {
        return mValue;
    }

    virtual sbfLogLevel getLevel()
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

    virtual void setLevel(sbfLogLevel level)
    {
        if (getHandle () != NULL)
            sbfLog_setLevel (getHandle (), level);
    }

    virtual bool setLevelFromString (const char* s)
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
    
    virtual const char* levelToString (sbfLogLevel level)
    {
        return sbfLog_levelToString (level);
    }

protected:
    sbfLog mValue;
};

}
