#ifndef _SBF_SHM_RING_BUFFER_PRIVATE_H_
#define _SBF_SHM_RING_BUFFER_PRIVATE_H_

SBF_BEGIN_DECLS

struct sbfShmRingBufferHeaderImpl
{
    sbfMutex mMutex;
    sbfCondVar mEnqueueCond;
    sbfCondVar mDequeueCond;
    size_t mHead;
    size_t mTail;
    size_t mSize;
    size_t mCapacity;
    size_t mElements;
    size_t mElementSize;
};

struct sbfShmRingBufferImpl
{
    int mFd;
    int mOwned;
    char* mFile;
    unsigned char* mBuffer;
    struct sbfShmRingBufferHeaderImpl* mHeader;
    int mDidSignal;
};

SBF_END_DECLS

#endif /* _SBF_SHM_RING_BUFFER_PRIVATE_H_ */
