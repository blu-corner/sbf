/*!
   \file sbfPool.h
   \brief This file declares the structures and functions to handle
   a memory pool.
   \copyright © Copyright 2016 Neueda all rights reserved.
 */

#ifndef _SBF_POOL_H_
#define _SBF_POOL_H_

#include "sbfCommon.h"
#include "sbfAtomic.h"

SBF_BEGIN_DECLS

/*! Declares the pool's handler. */
typedef struct sbfPoolImpl* sbfPool;

/*!
   Initialise the pool.
   \param log a log's handler.
 */
void sbfPool_init (sbfLog log);

/*!
   Creates the default memory pool with the given size.
   \param size the number of memory blocks in the pool.
   \return the default memory pool with the given size.
 */
sbfPool sbfPool_default (size_t size);

/*!
   Creates a pool for a given item size.
 */
#define sbfPool_create(itemSize) \
    sbfPool_create1 (itemSize, __FUNCTION__, __LINE__)

/*!
   \brief Creates a pool for a given item size and stores information
   about the function and line where it was created.
   \param itemSize the size of each item in the pool.
   \param function the name of the function.
   \param line the line number where the pool is calling the create method.
   \return a pool's handler.
 */
sbfPool sbfPool_create1 (size_t itemSize, const char* function, u_int line);

/*!
   \brief Releases all the resources allocated by the pool.
   \param pool the pool's handler.
 */
void sbfPool_destroy (sbfPool pool);

/*!
   Registers a hook to be invoked when exiting if the pool cannot be created.
   \param pool the pool's handler.
 */
void sbfPool_registerAtExit (sbfPool pool);

/*!
   Shows a summary of the defautl pool through the logging system.
   \param log the log's handler.
   \param level the log's level.
 */
void sbfPool_showSummary (sbfLog log, sbfLogLevel level);

/*!
   \brief Returns the size of the item defined for the given pool.
   \param pool the pool's handler.
   \return the size of the item defined for the given pool.
 */
static SBF_INLINE size_t sbfPool_getItemSize (sbfPool pool);

/*!
   Returns a block of memory available from the given pool.
   If the pool has no more memory blocks then NULL will be returned.
   \param pool the pool's handler.
   \return a block of memory available from the given pool.
 */
static SBF_INLINE void* sbfPool_get (sbfPool pool);

/*!
   Returns a block of memory available from the given pool and fills it
   with zeros.
   If the pool has no more memory blocks then NULL will be returned.
   \param pool the pool's handler.
   \return a block of memory available filled with zeros.
 */
static SBF_INLINE void* sbfPool_getZero (sbfPool pool);

/*!
   \brief Returns a block of memory to the pool.
   \param data the block of memory to be put into the pool.
 */
static SBF_INLINE void sbfPool_put (void* data);

#include "sbfPoolInline.h"

SBF_END_DECLS

#endif /* _SBF_POOL_H_ */
