/*!
   \file sbfCacheFile.h
   \brief This file declares the structures and functions to cache a file.
   \copyright © Copyright 2016 Neueda all rights reserved.
 */

#ifndef _SBF_CACHE_FILE_H_
#define _SBF_CACHE_FILE_H_

#include "sbfCommon.h"

SBF_BEGIN_DECLS

/*! Declaration of cache file's handler. */
typedef struct sbfCacheFileImpl* sbfCacheFile;
typedef struct sbfCacheFileItemImpl* sbfCacheFileItem;

/*!
   \brief Callback invoked when new file item is created.
   \param file the cache file's handler.
   \param item the item's handler.
   \param itemData the data linked to the file item.
   \param itemSize the size of the data linked to the file item.
   \param closure the user data linked to the cache file's handler.
   \return error code indicating if something went wrong during the
   processing of the callback.
 */
typedef sbfError (*sbfCacheFileItemCb) (sbfCacheFile file,
                                        sbfCacheFileItem item,
                                        void* itemData,
                                        size_t itemSize,
                                        void* closure);

/*!
   \brief Opens a memory mapped file descriptor to cache a file.
   item size.
   \param path the path to the file to be cached into memory.
   \param itemSize the size of the item.
   \param always_create if positive value it will force the overwrite of the
   file's content.
   \param created Will return possitive value if was able to create,
   0 otherwise.
   \param cb A callback indicating every cached file item created.
   \param closure a user data linked to the cache file's handler.
   \return a cache file's handler.
 */
sbfCacheFile sbfCacheFile_open (const char* path,
                                size_t itemSize,
                                int always_create,
                                int* created,
                                sbfCacheFileItemCb cb,
                                void* closure);

/*!
   Closes the cache file's handler.
   \param file the cache file's handler.
 */
void sbfCacheFile_close (sbfCacheFile file);

/*!
   Appends an item data to the cached file and returns the cache file item.
   \param file the cache file's handler.
   \param itemData pointer to data to be written.
   \return the cache file item linked to itemData.
 */
sbfCacheFileItem sbfCacheFile_add (sbfCacheFile file, void* itemData);

/*!
   Writes the given \itemData into a cache file item into
   \param item the item's handler.
   \param itemData the item data to be written.
   \return 0 for success, otherwise error code will be returned.
 */
sbfError sbfCacheFile_write (sbfCacheFileItem item, void* itemData);

/*!
   Writes the data at the given offset for the given cache file item.
   \param item the cache file item's handler.
   \param offset the offset to write the data.
   \param data the data to be written.
   \param size the size of the data to be written.
   \return 0 for success, otherwise error code will be returned.
 */
sbfError sbfCacheFile_writeOffset (sbfCacheFileItem item,
                                   size_t offset,
                                   void* data,
                                   size_t size);

/*!
   Flushes the cached changes to disk.
   \param file the cache file's handler.
 */
void sbfCacheFile_flush (sbfCacheFile file);

SBF_END_DECLS

#endif /* _SBF_CACHE_FILE_H_ */
