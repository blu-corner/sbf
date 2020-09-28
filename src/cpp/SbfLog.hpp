#pragma once

#include "sbfLog.h"
#include "sbfLogFile.h"

namespace neueda
{

class SbfLog {
public:
    /*!
        \brief Constructs a log that outputs to stdout.
        \return A SbfLog object.
     */
    SbfLog ()
    {
        mValue = sbfLog_create (NULL, "%s", "");
    }

    /*!
        \brief Destructor that deletes the private log handler.
        \return None.
     */
    virtual ~SbfLog ()
    {
        if (getHandle () != NULL)
            sbfLog_destroy (getHandle ());
    }

    /*!
        \brief Logs a message with a given level into the log.
        \param level the log level.
        \param fmt the formatting string.
        \param ... the parameters for the formatting string.
        \return None.
    */
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

    /*!
        \brief Logs a chunk of binary data with a given level into the log.
        \param level the log level.
        \param buf the data to be logged.
        \param len the size of the data in bytes.
        \return None.
    */
    virtual void logData (sbfLogLevel level, void* buf, size_t len)
    {
        if (getHandle () == NULL)
            return;

        sbfLog_logData (getHandle (), level, buf, len);
    }

    /*!
        \brief Returns a handle to the private C log struct.
        \return Pointer to a struct sbfLogImpl.
     */
    virtual sbfLog getHandle ()
    {
        return mValue;
    }

    /*!
        \brief Returns the log's level.
        \return the log's level.
    */
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

    /*!
        \brief Sets the log's level
        \param[in] level The log's level.
        \return None.
    */
    virtual void setLevel(sbfLogLevel level)
    {
        if (getHandle () != NULL)
            sbfLog_setLevel (getHandle (), level);
    }

    /*!
        \brief Sets the log's level from a string.
        \param[in] level Null terminated string representation of the log level.
    */
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
    
    /*!
       \brief Return a string representation of an enum log level.
       \param[in] level The log level to conver to string.
       \return Null terminated string representation of the pass in level.
     */
    virtual const char* levelToString (sbfLogLevel level)
    {
        return sbfLog_levelToString (level);
    }

protected:
    sbfLog mValue;
};

}
