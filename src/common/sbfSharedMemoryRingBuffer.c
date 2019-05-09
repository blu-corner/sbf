#include "sbfSharedMemoryRingBuffer.h"
#include "sbfSharedMemoryRingBufferPrivate.h"


static const char* SBF_SHARED_MEMOERY_ERRORS[] = {
    "shared-memory-create file already exists",
    "shared-memory-attach file doesnt exist"
};

#define SBF_SHM_CREATE_FILE_ALREADY_EXISTS  SBF_SHARED_MEMOERY_ERRORS[0]
#define SBF_SHM_ATTACH_FILE_DOESNT_EXIST    SBF_SHARED_MEMOERY_ERRORS[1]

static const char SBF_SHARED_MEMORY_TRAILER[] = "SBF_SHARED_MEMORY";


sbfShmMemoryRingBuffer
sbfShmMemoryRingBuffer_create (const char* path,
                               size_t elementSize,
                               size_t numberElements,
                               const char ** errorText)
{
    int alignedElementSize = elementSize + (elementSize % (sizeof (int)));
    int totalSize = sizeof (struct sbfShmRingBufferHeaderImpl)
        + (alignedElementSize * numberElements);

    int fd = -1;
    void* addr;
    if (path == NULL)  {
        addr = mmap (NULL, totalSize, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    } else {
        // check already exists
        if (access (path, F_OK) != -1) {
            *errorText = SBF_SHM_CREATE_FILE_ALREADY_EXISTS;
            return NULL;
        }

        // create the file
        fd = open (path, O_RDWR | O_CREAT | O_TRUNC, 0600);
        if (fd == -1) {
            *errorText = strerror (errno);
            return NULL;
        }

        // offset to shared memory size
        off_t offset = lseek (fd, totalSize, SEEK_SET);
        if (offset == (off_t) -1) {
            *errorText = strerror (errno);
            close (fd);
            return NULL;
        }

        // write a trailer to ensure the length
        if (write(fd, SBF_SHARED_MEMORY_TRAILER, sizeof (SBF_SHARED_MEMORY_TRAILER)) == -1) {
            *errorText = strerror (errno);
            close (fd);
            return NULL;
        }

        // shared memory address
        addr = mmap (NULL, totalSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    }

    if (addr == (void*)-1) {
        *errorText = strerror (errno);
        if (fd != -1)
            close (fd);
        return NULL;
    }
    
    struct sbfShmRingBufferHeaderImpl* header = (struct sbfShmRingBufferHeaderImpl*)addr;
    header->mHead = 0;
    header->mTail = 0;
    header->mSize = totalSize;
    header->mCapacity = numberElements;
    header->mElements = 0;
    header->mElementSize = alignedElementSize;
    
    // init the mutex
    sbfMutexAttr attr;
    sbfMutexAttr_init (&attr);
    sbfMutexAttr_setpshared (&attr, PTHREAD_PROCESS_SHARED);
    sbfMutex_init_attr (&(header->mMutex), &attr);

    // init cond
    sbfCondAttr condAttr;
    sbfCondAttr_init (&condAttr);
    sbfCondAttr_setpshared (&condAttr, PTHREAD_PROCESS_SHARED);
    sbfCondVar_init_attr (&(header->mEnqueueCond), &condAttr);
    sbfCondVar_init_attr (&(header->mDequeueCond), &condAttr);

    unsigned char* ringBuffer = (unsigned char*)addr + sizeof(struct sbfShmRingBufferHeaderImpl);
    
    sbfShmMemoryRingBuffer buffer;
    buffer = xmalloc (sizeof (*buffer));
    buffer->mFile = xstrdup (path);
    buffer->mFd = fd;
    buffer->mHeader = header;
    buffer->mBuffer = ringBuffer;
    buffer->mOwned = 1;
    buffer->mDidSignal = 0;

    return buffer;
}

sbfShmMemoryRingBuffer
sbfShmMemoryRingBuffer_attach (const char* path,
                               size_t elementSize,
                               size_t numberElements,
                               const char ** errorText)
{
    int alignedElementSize = elementSize + (elementSize % (sizeof (int)));
    int totalSize = sizeof (struct sbfShmRingBufferHeaderImpl)
        + (alignedElementSize * numberElements);

    // file needs to exist
    if (access (path, W_OK) != 0) {
        *errorText = SBF_SHM_ATTACH_FILE_DOESNT_EXIST;
        return NULL;
    }

    // open the file
    int fd = open (path, O_RDWR, 0600);
    if (fd == -1) {
        *errorText = strerror (errno);
        return NULL;
    }

    // shared memory address
    void* addr = mmap (NULL, totalSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (addr == (void*)-1) {
        *errorText = strerror (errno);
        close (fd);
        return NULL;
    }

    unsigned char* ringBuffer = (unsigned char*)addr + sizeof(struct sbfShmRingBufferHeaderImpl);

    sbfShmMemoryRingBuffer buffer;
    buffer = xmalloc (sizeof (*buffer));
    buffer->mFile = xstrdup (path);
    buffer->mFd = fd;
    buffer->mHeader = (struct sbfShmRingBufferHeaderImpl*)addr;
    buffer->mBuffer = ringBuffer;
    buffer->mOwned = 0;
    buffer->mDidSignal = 0;

    return buffer;
}

static void
sbfShmMemoryRingBuffer_signal (sbfShmMemoryRingBuffer buffer)
{
    buffer->mDidSignal = 1;
    sbfCondVar_signal (&(buffer->mHeader->mEnqueueCond));
    sbfCondVar_signal (&(buffer->mHeader->mDequeueCond));
}

void
sbfShmMemoryRingBuffer_destroy (sbfShmMemoryRingBuffer buffer)
{
    // signal for the blocking api to allow them to escape
    sbfShmMemoryRingBuffer_signal (buffer);
    
    if (buffer->mFile != NULL)
        free (buffer->mFile);
    
    if (buffer->mFd != -1)
        close (buffer->mFd);
    
    munmap (buffer->mHeader, buffer->mHeader->mSize);
    free (buffer);
}

size_t
sbfShmMemoryRingBuffer_size (sbfShmMemoryRingBuffer buffer)
{
    return buffer->mHeader->mElements;
}

size_t
sbfShmMemoryRingBuffer_capacity (sbfShmMemoryRingBuffer buffer)
{
    return buffer->mHeader->mSize;
}

int
sbfShmMemoryRingBuffer_empty (sbfShmMemoryRingBuffer buffer)
{
    return buffer->mHeader->mElements == 0;
}

int
sbfShmMemoryRingBuffer_isFileBased (sbfShmMemoryRingBuffer buffer)
{
    return buffer->mFile != NULL;
}

static void
sbfShmMemoryRingBuffer_doEnqueue (sbfShmMemoryRingBuffer buffer,
                                  const void* restrict data,
                                  size_t length)
{
    unsigned char* row = buffer->mBuffer + (buffer->mHeader->mTail * buffer->mHeader->mElementSize);
    
    // handle overflow case - truncate
    size_t copyLength = length > buffer->mHeader->mElementSize ?
        buffer->mHeader->mElementSize:
        length;

    // clear the row if the data is not going to be fully overwritten
    if (copyLength != length)
        memset (row, 0, buffer->mHeader->mElementSize);
    
    // copy in the data
    memcpy (row, data, copyLength);

    // update info
    buffer->mHeader->mElements += 1;
    buffer->mHeader->mTail = (buffer->mHeader->mTail + 1) % buffer->mHeader->mCapacity;
}

static void
sbfShmMemoryRingBuffer_doDequeue (sbfShmMemoryRingBuffer buffer,
                                  void* restrict data,
                                  size_t length)
{
    unsigned char* row = buffer->mBuffer + (buffer->mHeader->mHead * buffer->mHeader->mElementSize);

    // handle overflow case
    size_t copyLength = buffer->mHeader->mElementSize > length ?
        length :
        buffer->mHeader->mElementSize;

    // clear the data if its not going to be fully overriten
    if (copyLength != length)
        memset (data, 0, length);
    
    // copy the element
    memcpy (data, row, copyLength);

    // update info
    buffer->mHeader->mElements -= 1;
    buffer->mHeader->mHead = (buffer->mHeader->mHead + 1) % buffer->mHeader->mCapacity;
}

int
sbfShmMemoryRingBuffer_enqueue (sbfShmMemoryRingBuffer buffer,
                                const void* restrict data,
                                size_t length)
{
    sbfMutex_lock (&(buffer->mHeader->mMutex));
    
    if ((buffer->mHeader->mElements + 1) >= buffer->mHeader->mCapacity) {
        // not enough space
        sbfMutex_unlock (&(buffer->mHeader->mMutex));
        return -1;
    }

    // check element size
    if (length >= (buffer->mHeader->mElementSize)) {
        // element size too small
        sbfMutex_unlock (&(buffer->mHeader->mMutex));
        return -2;
    }

    sbfShmMemoryRingBuffer_doEnqueue (buffer, data, length);

    sbfCondVar_signal (&(buffer->mHeader->mDequeueCond));
    sbfMutex_unlock (&(buffer->mHeader->mMutex));
    
    return 0;
}

int
sbfShmMemoryRingBuffer_dequeue (sbfShmMemoryRingBuffer buffer,
                                void* restrict data,
                                size_t length)
{
    sbfMutex_lock (&(buffer->mHeader->mMutex));
    
    if (buffer->mHeader->mElements == 0) {
        // no eleements
        sbfMutex_unlock (&(buffer->mHeader->mMutex));
        return -1;
    }

    sbfShmMemoryRingBuffer_doDequeue (buffer, data, length);

    sbfCondVar_signal (&(buffer->mHeader->mEnqueueCond));
    sbfMutex_unlock (&(buffer->mHeader->mMutex));
    
    return 0;
}

int
sbfShmMemoryRingBuffer_dequeueAllocated (sbfShmMemoryRingBuffer buffer,
                                         void** data,
                                         size_t* length)
{
    *length = buffer->mHeader->mElementSize;
    *data = xmalloc (*length);

    return sbfShmMemoryRingBuffer_dequeue (buffer, *data, *length);
}

int
sbfShmMemoryRingBuffer_blockingEnqueue (sbfShmMemoryRingBuffer buffer,
                                        const void* restrict data,
                                        size_t length)
{
    sbfMutex_lock (&(buffer->mHeader->mMutex));
    if ((buffer->mHeader->mElements + 1) > buffer->mHeader->mCapacity && !(buffer->mDidSignal))
        sbfCondVar_wait (&(buffer->mHeader->mEnqueueCond), &(buffer->mHeader->mMutex));

    if (buffer->mDidSignal)
        return -5;

    // check element size
    if (length >= (buffer->mHeader->mElementSize)) {
        // element size too small
        sbfMutex_unlock (&(buffer->mHeader->mMutex));
        return -1;
    }

    sbfShmMemoryRingBuffer_doEnqueue (buffer, data, length);

    sbfCondVar_signal (&(buffer->mHeader->mDequeueCond));
    sbfMutex_unlock (&(buffer->mHeader->mMutex));
    
    return 0;
}

int
sbfShmMemoryRingBuffer_blockingDequeue (sbfShmMemoryRingBuffer buffer,
                                        void* restrict data,
                                        size_t length)
{
    sbfMutex_lock (&(buffer->mHeader->mMutex));
    if (buffer->mHeader->mElements == 0 && !(buffer->mDidSignal))
        sbfCondVar_wait (&(buffer->mHeader->mDequeueCond), &(buffer->mHeader->mMutex));

    if (buffer->mDidSignal)
        return -5;

    sbfShmMemoryRingBuffer_doDequeue (buffer, data, length);

    sbfCondVar_signal (&(buffer->mHeader->mEnqueueCond));
    sbfMutex_unlock (&(buffer->mHeader->mMutex));
    
    return 0;
}

int
sbfShmMemoryRingBuffer_blockingDequeueAllocated (sbfShmMemoryRingBuffer buffer,
                                                 void** data,
                                                 size_t* length)
{
    *length = buffer->mHeader->mElementSize;
    *data = xmalloc (*length);

    return sbfShmMemoryRingBuffer_blockingDequeue (buffer, *data, *length);
}
