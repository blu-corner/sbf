#ifndef _SBF_COMMON_HANDLER_PRIVATE_H_
#define _SBF_COMMON_HANDLER_PRIVATE_H_

#include "sbfHandler.h"
#include "sbfInterface.h"
#include "sbfMw.h"
#include "sbfTport.h"
#include "sbfUdpMulticast.h"

SBF_BEGIN_DECLS

struct sbfMcastTopicResolverImpl
{
    sbfLog   mLog;

    uint32_t mBase;

    u_int    mFirstSize;
    u_int    mSecondSize;
    u_int    mThirdSize;

    u_int    mFirstShift;
    u_int    mSecondShift;
};

SBF_END_DECLS

#endif /* _SBF_COMMON_HANDLER_PRIVATE_H_ */
