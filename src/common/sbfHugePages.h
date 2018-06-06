/*!
   \file sbfHugePages.h
   \brief This file declares the structures and functions to manage huge pages.
   \Copyright 2014-2018 Neueda Ltd.
 */

#ifndef _SBF_HUGE_PAGES_H_
#define _SBF_HUGE_PAGES_H_

#include "sbfCommon.h"

SBF_BEGIN_DECLS

/*!
   \brief Allocate a buffer for a given size by using memory mapped files.
   \param log the log system's handler.
   \param size the size of the buffer to be allocated.
   \return the allocated buffer or NULL if it was not possible.
 */
void* sbfHugePages_allocate (sbfLog log, size_t size);

/*!
   \brief Releases the buffer.
   \param block the buffer.
   \param size the size of the buffer.
 */
void sbfHugePages_free (void* block, size_t size);

SBF_END_DECLS

#endif /* _SBF_HUGE_PAGES_H_ */
