/*!
   \file sbfPath.h
   \brief Helper file to handle file system's paths.
   \copyright © Copyright 2016 Neueda all rights reserved.
 */

#ifndef _SBF_PATH_H_
#define _SBF_PATH_H_

#include "sbfCommon.h"

SBF_BEGIN_DECLS

/*!
   \brief Returns the root path in the file system in a null terminated string.
   \return the root path in the file system in a null terminated string.
 */
const char* sbfPath_root (void);

/*!
   \brief Get path underneath root path, result must be freed by caller. Paths
   starting with / are copied but otherwise unmodified.
   \param fmt formatted string.
   \param ... variable argument list that will be used to compose the
   final string together with \fmt.
   \return path underneath root path.
 */
char* sbfPath_get (const char* fmt, ...) SBF_PRINTFLIKE (1, 2);

/*!
   \brief Get path underneath root path, result must be freed by caller. Paths
   starting with / are copied but otherwise unmodified.
   \param fmt formatted string.
   \param ap variable argument list that will be used to compose the
   final string together with \fmt.
   \return path underneath root path.
 */
char* sbfPath_getV (const char* fmt, va_list ap);

/*!
   \brief Get path underneath config path, result must be freed by caller. Paths
   starting with / are copied but otherwise unmodified.
   \param fmt formatted string.
   \param ... variable argument list that will be used to compose the
   final string together with \fmt.
   \return
 */
char* sbfPath_getConf (const char* fmt, ...);

SBF_END_DECLS

#endif /* _SBF_PATH_H_ */
