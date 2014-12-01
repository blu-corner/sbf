#ifndef _SBF_PROPERTIES_H_
#define _SBF_PROPERTIES_H_

#include "sbfCommon.h"
#include "sbfKeyValue.h"
#include "sbfLog.h"
#include "sbfPath.h"

SBF_BEGIN_DECLS

typedef sbfError (*sbfPropertiesGroupCb) (const char* name,
                                          sbfKeyValue table,
                                          void* closure);

sbfKeyValue sbfProperties_loadDefault (char** error);
sbfKeyValue sbfProperties_load (const char* file, char** error);

sbfKeyValue sbfProperties_filter (sbfKeyValue table, const char* prefix);
sbfKeyValue sbfProperties_filterV (sbfKeyValue table,
                                   const char* fmt, ...) SBF_PRINTFLIKE(2, 3);
sbfKeyValue sbfProperties_filterN (sbfKeyValue table, ...);

/*
 * Given properties like:
 *
 * groups.a.foo=1
 * groups.a.bar=2
 * groups.b.foo=3
 *
 * Fire the callback once for a and once for b.
 */
sbfError sbfProperties_getGroup (sbfKeyValue table,
                                 const char* group,
                                 sbfPropertiesGroupCb cb,
                                 void* closure);

void sbfProperties_log (sbfLog log,
                        sbfLogLevel level,
                        const char* prefix,
                        sbfKeyValue table);

SBF_END_DECLS

#endif /* _SBF_PROPERTIES_H_ */
