#ifndef _SBF_CPU_FREQ_H_
#define _SBF_CPU_FREQ_H_

#include "sbfCommon.h"

SBF_BEGIN_DECLS

/*
 * Returns CPU frequency in MHz. Uses a uint64_t to make calculations cleaner
 * in the caller, even though no CPU will be that fast.
 */
uint64_t sbfCpuFreq_get (void);

SBF_END_DECLS

#endif /* _SBF_CPU_FREQ_H_ */
