/*!
   \file sbfLog.h
   \brief This file declares the structures and functions of a log system.
   \Copyright 2014-2018 Neueda Ltd.
 */

#ifndef _SBF_LOG_H_
#define _SBF_LOG_H_

#include "sbfCommon.h"

SBF_BEGIN_DECLS

struct sbfLogFileImpl;

/*! The log levels. */
typedef enum
{
    SBF_LOG_DEBUG = 0,
    SBF_LOG_INFO,
    SBF_LOG_WARN,
    SBF_LOG_ERROR,
    SBF_LOG_OFF
} sbfLogLevel;

/*! Declaration of the log's handler. */
typedef struct sbfLogImpl* sbfLog;

/* . */
/*!
   \brief This callback is invoked when hook is set to the log system to intercept
   logging messages.
   \param log the log's handler.
   \param level the log level.
   \param message the message that was hooked.
   \param closure the user data linked to the log's hook.
   \return 1 to skip the log, other value to continue.
 */
typedef int (*sbfLogHookCb) (sbfLog log,
                             sbfLogLevel level,
                             const char* message,
                             void* closure);

/*!
   \brief Creates a log through a formatted string.
   \param lf The log file's handler.
   \param fmt the formatted string.
   \param ... the list of arguments for the formatted string.
   \return the log's handler.
 */
sbfLog sbfLog_create (struct sbfLogFileImpl* lf,
                      const char* fmt,
                      ...) SBF_PRINTFLIKE(2, 3);

/*!
   \brief Releases all the resources allocated by the log's handler.
   \param log the log's handler.
 */
void sbfLog_destroy (sbfLog log);

/*!
   \brief Returns the log's level.
   \param log the log's handler.
   \return the log's level.
 */
sbfLogLevel sbfLog_getLevel (sbfLog log);

/*!
   \brief Sets the log's level
   \param log the log's handler.
   \param level the log's level.
 */
void sbfLog_setLevel (sbfLog log, sbfLogLevel level);

/*!
   \brief Converts from string level into level code.
   In the case that level string is not defined then SBF_LOG_OFF
   will be returned.
   \param s null terminated string containing the log level.
   \param found 1 if found, 0 if not found.
   \return the level code from a given null terminated string level.
 */
sbfLogLevel sbfLog_levelFromString (const char* s, int* found);

/*!
   \brief Converts from a code level into null terminated string level.
   \param level the level code.
   \return null terminated string containing the level.
 */
const char* sbfLog_levelToString (sbfLogLevel level);

/*! Helper macros to log usign different levels. */
#define sbfLog_err(log, fmt, ...) \
    sbfLog_log (log, SBF_LOG_ERROR, fmt, ## __VA_ARGS__)
#define sbfLog_warn(log, fmt, ...) \
    sbfLog_log (log, SBF_LOG_WARN, fmt, ## __VA_ARGS__)
#define sbfLog_info(log, fmt, ...) \
    sbfLog_log (log, SBF_LOG_INFO, fmt, ## __VA_ARGS__)
#define sbfLog_debug(log, fmt, ...) \
    sbfLog_log (log, SBF_LOG_DEBUG, fmt, ## __VA_ARGS__)

/*!
   \brief Logs a message with a given level into the logging system.
   \param log the log's handler.
   \param level the log level.
   \param fmt the formatting string.
   \param ... the parameters for the formatting string.
 */
void sbfLog_log (sbfLog log,
                 sbfLogLevel level,
                 const char* fmt,
                 ...) SBF_PRINTFLIKE(3, 4);

/*!
   \brief Logs a message with a given level into the logging system.
   \param log the log's handler.
   \param level the log level.
   \param fmt the formatting string.
   \param ap the list of arguments for the formatting string.
 */
void sbfLog_vlog (sbfLog log, sbfLogLevel level, const char* fmt, va_list ap);

/*!
   \brief Logs a chunk of binary data with a given level
   into the logging system.
   \param log the log's handler.
   \param level the log level.
   \param buf the data to be logged.
   \param len the size of the data in bytes.
 */
void sbfLog_logData (sbfLog log, sbfLogLevel level, void* buf, size_t len);

/*!
   \brief Sets a hook to the logging system to intercept the writing of log
   messages.
   \param log the log's handler.
   \param level the log's level.
   \param cb the callback to be invoked before the log is applied.
   \param closure the user data linked to the hook.
 */
void sbfLog_setHook (sbfLog log,
                     sbfLogLevel level,
                     sbfLogHookCb cb,
                     void* closure);

SBF_END_DECLS

#endif /* _SBF_LOG_H_ */
