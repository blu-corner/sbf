/*!
   \file sbfBuffer.h
   \brief This file defines the helper structures and functions to handle
   buffers.
   \Copyright 2014-2018 Neueda Ltd.
 */

#ifndef _SBF_BUFFER_H_
#define _SBF_BUFFER_H_

#include "sbfCommon.h"
#include "sbfPool.h"
#include "sbfRefCount.h"

SBF_BEGIN_DECLS

/*! Declare the buffer handler */
typedef struct sbfBufferImpl* sbfBuffer;

typedef void (*sbfBufferDestroyCb) (sbfBuffer buffer,
                                    void* data,
                                    void* closure);

//------------------------------------------------------------------------------
// Facade methods for pool
//------------------------------------------------------------------------------

/*! \see sbfPool_create1 */
#define sbfBuffer_createPool(size) \
    sbfBuffer_createPool1 (size, __FUNCTION__, __LINE__)

/*! \see sbfPool_create1 */
sbfPool sbfBuffer_createPool1 (size_t size, const char* function, u_int line);

/*! \see sbfPool_default */
sbfPool sbfBuffer_defaultPool (size_t size);

//------------------------------------------------------------------------------
// Public methods
//------------------------------------------------------------------------------

/*!
   \brief Returns a memory buffer of a given size.
   If a pool handler is provided at \p pool, the memory buffer will be obtained
   from the pool, otherwise the buffer will be allocated and returned.
   \param pool The pools handler.
   \param size the size of the buffer to be allocated.
   \return a buffer's handler.
 */
static SBF_INLINE sbfBuffer sbfBuffer_new (sbfPool pool, size_t size);

/*!
   \brief Returns a memory buffer with the given size by filling it with zeros.
   If a pool handler is provided at \p pool, the memory buffer will be obtained
   from the pool, otherwise the buffer will be allocated and returned.
   \param pool The pools handler.
   \param size the size of the buffer to be allocated.
   \return a buffer's handler.
 */
static SBF_INLINE sbfBuffer sbfBuffer_newZero (sbfPool pool, size_t size);

/*!
   \brief Creates a buffer handler by copying the provided memory.
   If a pool handler is provided at \p pool, the memory buffer will be obtained
   from the pool, otherwise the buffer will be allocated and returned.
   \param pool The pools handler.
   \param data the data to be linked to the buffer.
   \param size the size of the buffer to be allocated.
   \return
 */
static SBF_INLINE sbfBuffer sbfBuffer_copy (sbfPool pool,
                                            void* data,
                                            size_t size);

/*!
   \brief It uses an internal pool so that provided memory is linked to the
   buffer so that no copy is performed.
   When the buffer is destroyed then the provided callback will be invoked
   to release the data.
   \param data the memory to be wrapped.
   \param size the size of the memory.
   \param cb a callback that will be invoked when the buffer is destroyed.
   \param closure a user data linked to the returned buffer's handler.
   \return a buffer handler.
 */
static SBF_INLINE sbfBuffer sbfBuffer_wrap (void* data,
                                            size_t size,
                                            sbfBufferDestroyCb cb,
                                            void* closure);

/*!
   \brief Increases the reference count for the buffer handler.
   \param buffer the buffer's handler.
   \see sbfBuffer_destroy
 */
static SBF_INLINE void sbfBuffer_addRef (sbfBuffer buffer);

/*!
   \brief Blocks the buffer to avoid content modifications. If the caller tries to
   modify the buffer after a lock operation, the program will exit.
   \param buffer the buffer's handler.
 */
static SBF_INLINE void sbfBuffer_lock (sbfBuffer buffer);

/*!
   \brief Decreases the reference count and releases the resources allocated
   by the buffer when counts come down to zero.
   \param buffer the buffer's handler.
 */
static SBF_INLINE void sbfBuffer_destroy (sbfBuffer buffer);

/*!
   \brief Returns the data allocated by the buffer.
   \param buffer the buffer's handler.
   
eturn
 */
static SBF_INLINE void* sbfBuffer_getData (sbfBuffer buffer);

/*!
   \brief Links a data to the given buffer's handler (no copy).
   \param buffer the buffer's handler.
   \param data the data to be linked to the buffer.
 */
static SBF_INLINE void sbfBuffer_setData (sbfBuffer buffer, void* data);

/*!
   \brief Returns the size of the buffer.
   \param buffer the buffer's handler.
   \return the size of the buffer.
 */
static SBF_INLINE size_t sbfBuffer_getSize (sbfBuffer buffer);

/*!
   \brief Sets the size of the buffer (no allocation of memory is performed).
   \param buffer the buffer's handler.
   \param size the size of the buffer.
 */
static SBF_INLINE void   sbfBuffer_setSize (sbfBuffer buffer, size_t size);

#include "sbfBufferInline.h"

extern sbfPool gSbfBufferDefaultPool;

SBF_END_DECLS

#endif /* _SBF_BUFFER_H_ */
