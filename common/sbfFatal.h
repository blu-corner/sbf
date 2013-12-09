#ifndef _SBF_FATAL_H_
#define _SBF_FATAL_H_

#include "sbfCommon.h"

SBF_BEGIN_DECLS

void sbfFatal (const char* s);
#define SBF_FATAL(s) sbfFatal (s)

SBF_END_DECLS

#endif /* _SBF_FATAL_H_ */
