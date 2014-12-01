#ifndef _SBF_FATAL_H_
#define _SBF_FATAL_H_

#include "sbfCommon.h"

SBF_BEGIN_DECLS

SBF_DEAD void sbfFatal_die (const char* msg);
#define SBF_FATAL(msg) sbfFatal_die (msg)

SBF_END_DECLS

#endif /* _SBF_FATAL_H_ */
