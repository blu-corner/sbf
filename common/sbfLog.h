#ifndef _SBF_LOG_H_
#define _SBF_LOG_H_

#include "sbfCommon.h"

SBF_BEGIN_DECLS

typedef enum
{
    SBF_LOG_DEBUG = 0,
    SBF_LOG_INFO,
    SBF_LOG_WARN,
    SBF_LOG_ERROR,
    SBF_LOG_OFF
} sbfLogLevel;

typedef void (*sbfLogCb) (sbfLogLevel level,
                          const char* message,
                          void* closure);

void sbfLog_setLevel (sbfLogLevel level);
sbfLogLevel sbfLog_levelFromString (const char* s);
void sbfLog_setCallback (sbfLogCb cb, void* closure);

#define sbfLog_err(fmt, ...) \
    sbfLog_log (SBF_LOG_ERROR, "%s: " fmt, __func__, ## __VA_ARGS__)
#define sbfLog_warn(fmt, ...) \
    sbfLog_log (SBF_LOG_WARN, "%s: " fmt, __func__, ## __VA_ARGS__)
#define sbfLog_info(fmt, ...) \
    sbfLog_log (SBF_LOG_INFO, "%s: " fmt, __func__, ## __VA_ARGS__)
#define sbfLog_debug(fmt, ...) \
    sbfLog_log (SBF_LOG_DEBUG, "%s: " fmt, __func__, ## __VA_ARGS__)

void sbfLog_log (sbfLogLevel level, const char* fmt, ...) SBF_PRINTFLIKE(2, 3);
void sbfLog_vlog (sbfLogLevel level, const char* fmt, va_list ap);
void sbfLog_logData (sbfLogLevel level, void* buf, size_t len);

SBF_END_DECLS

#endif /* _SBF_LOG_H_ */
