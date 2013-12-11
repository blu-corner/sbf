#ifndef _SBF_COMMON_H_
#define _SBF_COMMON_H_

#include "sbfVersion.h"

#ifdef  __cplusplus
#define SBF_BEGIN_DECLS extern "C" {
#define SBF_END_DECLS }
#else
#define SBF_BEGIN_DECLS
#define SBF_END_DECLS
#endif

#ifdef linux
#include "sbfCommonLinux.h"
#endif

#ifdef WIN32
#include "sbfCommonWin32.h"
#endif

#include "syshash.h"
#include "sysqueue.h"
#include "systree.h"

#include "sbfLog.h"
#include "sbfFatal.h"
#include "sbfMemory.h"

SBF_BEGIN_DECLS

#define SBF_MIN(a, b) ((a) > (b) ? (b) : (a))
#define SBF_MAX(a, b) ((a) < (b) ? (b) : (a))

typedef int sbfError; /* errno return */

SBF_END_DECLS

#endif /* _SBF_COMMON_H_ */
