#ifndef _SBF_NUMBER_STRING_H_
#define _SBF_NUMBER_STRING_H_

#include "sbfCommon.h"

SBF_BEGIN_DECLS

int64_t sbfNumberString_toInt64 (const char* s,
                                 int64_t min,
                                 int64_t max,
                                 const char** error);
double sbfNumberString_toDouble (const char* s, const char** error);

SBF_END_DECLS

#endif /* _SBF_NUMBER_STRING_H_ */
