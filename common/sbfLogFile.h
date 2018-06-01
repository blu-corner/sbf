/*!
   \file sbfLogFile.h
   \brief This file declares the structures and functions of a log file.
   \Copyright 2014-2018 Neueda Ltd.
 */

#ifndef _SBF_LOG_FILE_H_
#define _SBF_LOG_FILE_H_

#include "sbfCommon.h"

SBF_BEGIN_DECLS

/*! Declares a log file's handler. */
typedef struct sbfLogFileImpl* sbfLogFile;

/*! Maximum number of log file entries. */
#define SBF_LOG_FILE_ENTRIES 200000

/*! Log file's handler implementation */
SBF_PACKED (struct sbfLogFileEntryImpl
{
    u_int          mNumber;

    struct timeval mTime;
    uint8_t        mLevel;
    char           mPrefix[24];
    char           mMessage[512];
});

/*! Declaration of log file's handler. */
typedef struct sbfLogFileEntryImpl* sbfLogFileEntry;

/*!
   \brief Returns the size of the log.
   \return the size of the log.
 */
size_t sbfLogFile_size (void);

/*!
   \brief Creates a log file's handler from a given path and formatted string.
   \param error Null terminated string holding the error cause if any.
   \param root the directory to place the files.
   \param fmt format string to compose the file.
   \param ... variable arguments for the format string.
   \return the log's file handler.
 */
sbfLogFile sbfLogFile_create (char** error,
                              const char* root,
                              const char* fmt,
                              ...) SBF_PRINTFLIKE (3, 4);

/*!
   \brief Opens a log file's handler from a given path and formatted string.
   \param error Null terminated string holding the error cause if any.
   \param root the directory to place the files.
   \param fmt format string to compose the file.
   \param ... variable arguments for the format string.
   \return the log's file handler.
 */
sbfLogFile sbfLogFile_open (char** error,
                            const char* root,
                            const char* fmt,
                            ...) SBF_PRINTFLIKE (3, 4);

/*!
   \brief Closes the log file.
   \param lf the log file's handler.
 */
void sbfLogFile_close (sbfLogFile lf);

/*!
   \brief Returns a log file entry from a given log file's handler.
   \param lf the log file's handler.
   \return a log file entry from a given log file's handler.
 */
sbfLogFileEntry sbfLogFile_get (sbfLogFile lf);

/*!
   \brief Makes the log file to flush the content on the file system.
   \param lf the log file's handler.
   \param lfe the log file entry's handler.
 */
void sbfLogFile_flush (sbfLogFile lf, sbfLogFileEntry lfe);

/*!
   \brief Reads a log entry from the log file's handler.
   \param lf the log file's handler.
   \param number the number of log item to be read. If read is successfull then
   it will increase the number by one.
   \return a log entry from the log file's handler.
 */
sbfLogFileEntry sbfLogFile_read (sbfLogFile lf, u_int* number);

SBF_END_DECLS

#endif /* _SBF_LOG_FILE_H_ */
