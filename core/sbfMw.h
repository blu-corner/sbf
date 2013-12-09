#ifndef _SBF_MW_H_
#define _SBF_MW_H_

#include "sbfCommon.h"
#include "sbfEvent.h"
#include "sbfQueue.h"
#include "sbfTimer.h"

SBF_BEGIN_DECLS

typedef struct sbfMwImpl* sbfMw;
typedef struct sbfMwThreadImpl* sbfMwThread;

#define SBF_MW_THREAD_LIMIT 64
#define SBF_MW_ALL_THREADS (~0ULL)

sbfMw sbfMw_create (u_int threads);
void sbfMw_destroy (sbfMw mw);

u_int sbfMw_getNumThreads (sbfMw mw);
sbfMwThread sbfMw_getThread (sbfMw mw, u_int index);
#define sbfMw_getDefaultThread(mw) sbfMw_getThread (mw, 0)

u_int sbfMw_getThreadIndex (sbfMwThread thread);
uint64_t sbfMw_getThreadMask (sbfMwThread thread);

SBF_END_DECLS

#endif /* _SBF_MW_H_ */
