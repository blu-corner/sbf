/*!
   \file sbfPool.h
   \brief This file declares the structures and functions to handle
   a property file.
   \Copyright 2014-2018 Neueda Ltd.
 */

#ifndef _SBF_PROPERTIES_H_
#define _SBF_PROPERTIES_H_

#include "sbfCommon.h"
#include "sbfKeyValue.h"
#include "sbfLog.h"
#include "sbfPath.h"

SBF_BEGIN_DECLS

/*!
   Callback that is invoked when getGroup is invoked to give the opportunity to
   interrupt the retrieval of the group.
   \param name the name of the property being scanned.
   \param table the key value table's handler.
   \param closure the user data linked to the property handler.
   \return 0 if you want the getGroup operation to continue,
   error code otherwise.
 */
typedef sbfError (*sbfPropertiesGroupCb) (const char* name,
                                          sbfKeyValue table,
                                          void* closure);

/*!
   Loads a key value table's handler containing the set of properties
   by default.
   \param error null terminated string in case that error happens during
   the loading.
   \return a key value table's handler containing the set of properties.
 */
sbfKeyValue sbfProperties_loadDefault (char** error);

/*!
   Loads a key value table's handler containing the set of properties
   for the given filename.
   \param file null terminated string with the properties' filename.
   \param error null terminated string in case that error happens during
   the loading.
   \return a key value table's handler containing the set of properties.
 */
sbfKeyValue sbfProperties_load (const char* file, char** error);

/*!
   Returns the properties which key matches with the given prefix.
   \param table the key value table's handler holding the properties.
   \param prefix the prefix to filter the properties.
   \return the properties which key matches with the given prefix.
 */
sbfKeyValue sbfProperties_filter (sbfKeyValue table, const char* prefix);

/*!
   Returns the properties which key matches with the given prefix.
   \param table the key value table's handler holding the properties.
   \param fmt formatted string to build the filter.
   \param ... variable number of arguments to compose the filter.
   \return the properties which key matches with the given prefix.
 */
sbfKeyValue sbfProperties_filterV (sbfKeyValue table,
                                   const char* fmt, ...) SBF_PRINTFLIKE(2, 3);
/*!
   Returns the properties which key matches with the given list of prefixes.
   \param table the key value table's handler holding the properties.
   \param ... the list of prefixes to filter the properties.
   \return the properties which key matches with the given prefixes.
 */
sbfKeyValue sbfProperties_filterN (sbfKeyValue table, ...);

/*!
   Fires a callback for the detected groups. If callback returns errors the
   scanning for more groups will be interrupted.
 *
   Given properties like:
 *
   groups.a.foo=1
   groups.a.bar=2
   groups.b.foo=3
 *
   Fire the callback once for a and once for b.
   \param table the key value table's handler holding the properties.
   \param group the parent group to be scanned.
   \param cb callback that will be invoked for every detected property group
   inside the given parent group.
   \param closure the user data linked to the callback.
   \return 0 if successfully done, error code otherwise.
 */
sbfError sbfProperties_getGroup (sbfKeyValue table,
                                 const char* group,
                                 sbfPropertiesGroupCb cb,
                                 void* closure);

/*!
   Logs a given property table into the given log.
   \param log the log's handler.
   \param level the log's level.
   \param prefix the prefix matching the properties to be logged.
   \param table the key value table's handler containing the properties.
 */
void sbfProperties_log (sbfLog log,
                        sbfLogLevel level,
                        const char* prefix,
                        sbfKeyValue table);

SBF_END_DECLS

#endif /* _SBF_PROPERTIES_H_ */
