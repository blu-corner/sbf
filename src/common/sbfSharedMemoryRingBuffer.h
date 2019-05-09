/*
   \file sbfSharedMemoryRingBuffer.h
   \brief This file declares the strcutures and functions to manage
   a shared memory ring buffer
   \Copyright 2014-2018 Neueda Ltd.
 */

#ifndef _SBF_SHARED_MEMORY_RING_BUFFER_H_
#define _SBF_SHARED_MEMORY_RING_BUFFER_H_

#include "sbfCommon.h"

SBF_BEGIN_DECLS

/*! Declares the handler. */
typedef struct sbfShmRingBufferImpl* sbfShmMemoryRingBuffer;

sbfShmMemoryRingBuffer sbfShmMemoryRingBuffer_create (
    const char* path,
    size_t elementSize,
    size_t numberElements,
    const char ** errorText);

sbfShmMemoryRingBuffer sbfShmMemoryRingBuffer_attach (
    const char* path,
    size_t elementSize,
    size_t numberElements,
    const char ** errorText);

void sbfShmMemoryRingBuffer_destroy (sbfShmMemoryRingBuffer buffer);

size_t sbfShmMemoryRingBuffer_size (sbfShmMemoryRingBuffer buffer);

size_t sbfShmMemoryRingBuffer_capacity (sbfShmMemoryRingBuffer buffer);

int sbfShmMemoryRingBuffer_empty (sbfShmMemoryRingBuffer buffer);

int sbfShmMemoryRingBuffer_isFileBased (sbfShmMemoryRingBuffer buffer);

int sbfShmMemoryRingBuffer_enqueue (sbfShmMemoryRingBuffer buffer,
                                    const void* data,
                                    size_t length);

int sbfShmMemoryRingBuffer_dequeueAllocated (sbfShmMemoryRingBuffer buffer,
                                             void** data,
                                             size_t* length);

int sbfShmMemoryRingBuffer_dequeue (sbfShmMemoryRingBuffer buffer,
                                    void* data,
                                    size_t length);

int sbfShmMemoryRingBuffer_blockingEnqueue (sbfShmMemoryRingBuffer buffer,
                                            const void* data,
                                            size_t length);

int sbfShmMemoryRingBuffer_blockingDequeueAllocated (sbfShmMemoryRingBuffer buffer,
                                                     void** data,
                                                     size_t* length);

int sbfShmMemoryRingBuffer_blockingDequeue (sbfShmMemoryRingBuffer buffer,
                                            void* data,
                                            size_t length);

SBF_END_DECLS

#endif /* _SBF_SHARED_MEMORY_RING_BUFFER_H_ */
