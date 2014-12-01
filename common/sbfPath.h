#ifndef _SBF_PATH_H_
#define _SBF_PATH_H_

#include "sbfCommon.h"

SBF_BEGIN_DECLS

const char* sbfPath_root (void);

/*
 * Get path underneath root path, result must be freed by caller. Paths
 * starting with / are copied but otherwise unmodified.
 */
char* sbfPath_get (const char* fmt, ...) SBF_PRINTFLIKE (1, 2);
char* sbfPath_getV (const char* fmt, va_list ap);

/*
 * Get path underneath config path, result must be freed by caller. Paths
 * starting with / are copied but otherwise unmodified.
 */
char* sbfPath_getConf (const char* fmt, ...);

SBF_END_DECLS

#endif /* _SBF_PATH_H_ */
