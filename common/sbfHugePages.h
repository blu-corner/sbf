#ifndef _SBF_HUGE_PAGES_H_
#define _SBF_HUGE_PAGES_H_

#include "sbfCommon.h"

SBF_BEGIN_DECLS

void* sbfHugePages_allocate (size_t size);
void sbfHugePages_free (void* block, size_t size);

SBF_END_DECLS

#endif /* _SBF_HUGE_PAGES_H_ */
