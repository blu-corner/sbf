#ifndef _SBF_COMMON_H_
#define _SBF_COMMON_H_

#include "sbfVersion.h"

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

typedef int sbfError; /* errno return */

SBF_END_DECLS

#endif /* _SBF_COMMON_H_ */
