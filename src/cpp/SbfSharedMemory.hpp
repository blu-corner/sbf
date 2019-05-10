#pragma once

#include "sbfShmRingBuffer.h"
#include <string>


namespace neueda
{
    class SbfShmRingBuffer {
    public:
        static SbfShmRingBuffer* create (const char* path,
                                         size_t elementSize,
                                         size_t numberElements,
                                         std::string& error) {
            const char* errorText = NULL;
            sbfShmRingBuffer buffer = sbfShmRingBuffer_create (path,
                                                               elementSize,
                                                               numberElements,
                                                               &errorText);
            if (buffer == NULL) {
                error.assign (errorText);
                return nullptr;
            }

            return new SbfShmRingBuffer (buffer);
        }

        static SbfShmRingBuffer* attach (const char* path,
                                         size_t elementSize,
                                         size_t numberElements,
                                         std::string& error) {
            const char* errorText = NULL;
            sbfShmRingBuffer buffer = sbfShmRingBuffer_attach (path,
                                                               elementSize,
                                                               numberElements,
                                                               &errorText);
            if (buffer == NULL) {
                error.assign (errorText);
                return nullptr;
            }

            return new SbfShmRingBuffer (buffer);
        }

        ~SbfShmRingBuffer() {
            sbfShmRingBuffer_destroy (mHandle);
        }
        
        size_t getSize () const {
            return sbfShmRingBuffer_size (mHandle);
        }

        size_t getCapacity () const {
            return sbfShmRingBuffer_capacity (mHandle);
        }

        bool empty () const {
            sbfShmRingBuffer_empty (mHandle) == 1;
        }

        bool isFileBased () const {
            return sbfShmRingBuffer_isFileBased (mHandle) == 1;
        }

        std::string getFilePath () const {
            const char* cpath = sbfShmRingBuffer_getFilePath (mHandle);

            std::string path;
            if (cpath != NULL) {
                path.assign (cpath)
            }
            
            return path;
        }

        bool enqueue (const void* data, size_t length) {
            return sbfShmRingBuffer_enqueue (data, length) == 0;
        }

        bool dequeue (void* data, size_t length) {
            return sbfShmRingBuffer_dequeue (data, length) == 0;
        }

        bool allocatedDequeue (void* data, size_t length) {
            return sbfShmRingBuffer_dequeueAllocated (data, length) == 0;
        }

        bool blockingEnqueue (const void* data, size_t length) {
            return sbfShmRingBuffer_blockingEnqueue (data, length) == 0;
        }

        bool blockingDequeue (void* data, size_t length) {
            return sbfShmRingBuffer_blockingDequeue (data, length) == 0;
        }

        bool blockingAllocatedDequeue (void* data, size_t length) {
            return sbfShmRingBuffer_blockingDequeueAllocated (data, length) == 0;
        }

    private:
        SbfShmRingBuffer (sbfShmRingBuffer handle)
            : mHandle (handle) { }
        
        sbfShmRingBuffer mHandle;
    };
}
