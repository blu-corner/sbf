#ifndef _SBF_LOG_PRIVATE_H_
#define _SBF_LOG_PRIVATE_H_

#include "sbfCommon.h"
#include "sbfLogFile.h"

SBF_BEGIN_DECLS

struct sbfLogImpl
{
    sbfLogFile   mFile;
    sbfLogLevel  mLevel;
    char*        mPrefix;

    sbfLogLevel  mHookLevel;
    sbfLogHookCb mHookCb;
    void*        mHookClosure;
    int          mHookInside;
};

SBF_END_DECLS

#endif /* _SBF_LOG_PRIVATE_H_ */
