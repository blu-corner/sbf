#ifndef _SBF_LOG_FILE_PRIVATE_H_
#define _SBF_LOG_FILE_PRIVATE_H_

#include "sbfCommon.h"
#include "sbfPath.h"

SBF_BEGIN_DECLS

struct sbfLogFileImpl
{
    const char*     mPath;

    sbfLogFileEntry mBase;
    u_int           mNext;
};

SBF_END_DECLS

#endif /* _SBF_LOG_FILE_PRIVATE_H_ */
