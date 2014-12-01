#ifndef _SBF_LOG_H_
#define _SBF_LOG_H_

#include "sbfCommon.h"

SBF_BEGIN_DECLS

struct sbfLogFileImpl;

typedef enum
{
    SBF_LOG_DEBUG = 0,
    SBF_LOG_INFO,
    SBF_LOG_WARN,
    SBF_LOG_ERROR,
    SBF_LOG_OFF
} sbfLogLevel;

typedef struct sbfLogImpl* sbfLog;

typedef void (*sbfLogHookCb) (sbfLog log,
                              sbfLogLevel level,
                              const char* message,
                              void* closure);

sbfLog sbfLog_create (struct sbfLogFileImpl* lf,
                      const char* fmt,
                      ...) SBF_PRINTFLIKE(2, 3);
void sbfLog_destroy (sbfLog log);

sbfLogLevel sbfLog_getLevel (sbfLog log);
void sbfLog_setLevel (sbfLog log, sbfLogLevel level);
sbfLogLevel sbfLog_levelFromString (const char* s, int* found);
const char* sbfLog_levelToString (sbfLogLevel level);

#define sbfLog_err(log, fmt, ...) \
    sbfLog_log (log, SBF_LOG_ERROR, fmt, ## __VA_ARGS__)
#define sbfLog_warn(log, fmt, ...) \
    sbfLog_log (log, SBF_LOG_WARN, fmt, ## __VA_ARGS__)
#define sbfLog_info(log, fmt, ...) \
    sbfLog_log (log, SBF_LOG_INFO, fmt, ## __VA_ARGS__)
#define sbfLog_debug(log, fmt, ...) \
    sbfLog_log (log, SBF_LOG_DEBUG, fmt, ## __VA_ARGS__)

void sbfLog_log (sbfLog log,
                 sbfLogLevel level,
                 const char* fmt,
                 ...) SBF_PRINTFLIKE(3, 4);
void sbfLog_vlog (sbfLog log, sbfLogLevel level, const char* fmt, va_list ap);
void sbfLog_logData (sbfLog log, sbfLogLevel level, void* buf, size_t len);

void sbfLog_setHook (sbfLog log,
                     sbfLogLevel level,
                     sbfLogHookCb cb,
                     void* closure);

SBF_END_DECLS

#endif /* _SBF_LOG_H_ */
