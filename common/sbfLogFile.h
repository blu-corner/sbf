#ifndef _SBF_LOG_FILE_H_
#define _SBF_LOG_FILE_H_

#include "sbfCommon.h"

SBF_BEGIN_DECLS

typedef struct sbfLogFileImpl* sbfLogFile;

#define SBF_LOG_FILE_ENTRIES 200000
SBF_PACKED (struct sbfLogFileEntryImpl
{
    u_int          mNumber;

    struct timeval mTime;
    uint8_t        mLevel;
    char           mPrefix[24];
    char           mMessage[512];
});
typedef struct sbfLogFileEntryImpl* sbfLogFileEntry;

size_t sbfLogFile_size (void);

sbfLogFile sbfLogFile_create (char** error,
                              const char* root,
                              const char* fmt,
                              ...) SBF_PRINTFLIKE (3, 4);
sbfLogFile sbfLogFile_open (char** error,
                            const char* root,
                            const char* fmt,
                            ...) SBF_PRINTFLIKE (3, 4);
void sbfLogFile_close (sbfLogFile lf);

sbfLogFileEntry sbfLogFile_get (sbfLogFile lf);
void            sbfLogFile_flush (sbfLogFile lf, sbfLogFileEntry lfe);
sbfLogFileEntry sbfLogFile_read (sbfLogFile lf, u_int* number);

SBF_END_DECLS

#endif /* _SBF_LOG_FILE_H_ */
