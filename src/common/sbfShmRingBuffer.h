/*
  \file sbfShmRingBuffer.h
  \brief This file declares the strcutures and functions to manage
  a shared memory ring buffer
  \Copyright 2014-2018 Neueda Ltd.
*/

#ifndef _SBF_SHM_RING_BUFFER_H_
#define _SBF_SHM_RING_BUFFER_H_

#include "sbfCommon.h"

SBF_BEGIN_DECLS

/*! Declares the handler. */
typedef struct sbfShmRingBufferImpl* sbfShmRingBuffer;

sbfShmRingBuffer sbfShmRingBuffer_create (
    const char* path,
    size_t elementSize,
    size_t numberElements,
    const char** errorText);

sbfShmRingBuffer sbfShmRingBuffer_attach (
    const char* path,
    size_t elementSize,
    size_t numberElements,
    const char** errorText);

void sbfShmRingBuffer_destroy (sbfShmRingBuffer buffer);

size_t sbfShmRingBuffer_size (sbfShmRingBuffer buffer);

size_t sbfShmRingBuffer_capacity (sbfShmRingBuffer buffer);

int sbfShmRingBuffer_empty (sbfShmRingBuffer buffer);

int sbfShmRingBuffer_isFileBased (sbfShmRingBuffer buffer);

const char* sbfShmRingBuffer_getFilePath (sbfShmRingBuffer buffer);

int sbfShmRingBuffer_enqueue (sbfShmRingBuffer buffer,
                              const void* data,
                              size_t length);

int sbfShmRingBuffer_dequeueAllocated (sbfShmRingBuffer buffer,
                                       void** data,
                                       size_t* length);

int sbfShmRingBuffer_dequeue (sbfShmRingBuffer buffer,
                              void* data,
                              size_t length);

int sbfShmRingBuffer_blockingEnqueue (sbfShmRingBuffer buffer,
                                      const void* data,
                                      size_t length);

int sbfShmRingBuffer_blockingDequeueAllocated (sbfShmRingBuffer buffer,
                                               void** data,
                                               size_t* length);

int sbfShmRingBuffer_blockingDequeue (sbfShmRingBuffer buffer,
                                      void* data,
                                      size_t length);

SBF_END_DECLS

#endif /* _SBF_SHM_RING_BUFFER_H_ */
