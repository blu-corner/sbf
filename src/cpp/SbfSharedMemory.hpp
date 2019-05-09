#pragma once

#include "sbfSharedMemoryRingBuffer.h"
#include <string>


namespace neueda
{
    class SbfSharedMemoryRingBuffer {
    public:
        static SbfSharedMemoryRingBuffer* create (const char* path,
                                                  size_t elementSize,
                                                  size_t numberElements,
                                                  std::string& error) {
            const char* errorText = NULL;
            sbfShmMemoryRingBuffer buffer = sbfShmMemoryRingBuffer_create (path,
                                                                           elementSize,
                                                                           numberElements,
                                                                           &errorText);
            if (buffer == NULL) {
                error.assign (errorText);
                return nullptr;
            }

            return new SbfSharedMemoryRingBuffer (buffer);
        }

        static SbfSharedMemoryRingBuffer* attach (const char* path,
                                                  size_t elementSize,
                                                  size_t numberElements,
                                                  std::string& error) {
            const char* errorText = NULL;
            sbfShmMemoryRingBuffer buffer = sbfShmMemoryRingBuffer_attach (path,
                                                                           elementSize,
                                                                           numberElements,
                                                                           &errorText);
            if (buffer == NULL) {
                error.assign (errorText);
                return nullptr;
            }

            return new SbfSharedMemoryRingBuffer (buffer);
        }

        ~SbfSharedMemoryRingBuffer() {
            sbfShmMemoryRingBuffer_destroy (mHandle);
        }
        
        size_t getSize () const {
            return sbfShmMemoryRingBuffer_size (mHandle);
        }

        size_t getCapacity () const {
            return sbfShmMemoryRingBuffer_capacity (mHandle);
        }

        bool empty () const {
            sbfShmMemoryRingBuffer_empty (mHandle) == 1;
        }

        bool isFileBased () const {
            return sbfShmMemoryRingBuffer_isFileBased (mHandle) == 1;
        }

        bool enqueue (const void* data, size_t length) {
            return sbfShmMemoryRingBuffer_enqueue (data, length) == 0;
        }

        bool dequeue (void* data, size_t length) {
            return sbfShmMemoryRingBuffer_dequeue (data, length) == 0;
        }

        bool allocatedDequeue (void* data, size_t length) {
            return sbfShmMemoryRingBuffer_dequeueAllocated (data, length) == 0;
        }

        bool blockingEnqueue (const void* data, size_t length) {
            return sbfShmMemoryRingBuffer_blockingEnqueue (data, length) == 0;
        }

        bool blockingDequeue (void* data, size_t length) {
            return sbfShmMemoryRingBuffer_blockingDequeue (data, length) == 0;
        }

        bool blockingAllocatedDequeue (void* data, size_t length) {
            return sbfShmMemoryRingBuffer_blockingDequeueAllocated (data, length) == 0;
        }

    private:
        SbfSharedMemoryRingBuffer (sbfShmMemoryRingBuffer handle)
            : mHandle (handle) { }
        
        sbfShmMemoryRingBuffer mHandle;
    };
}
