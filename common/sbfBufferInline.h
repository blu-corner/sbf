#ifndef _SBF_BUFFER_H_
#error "must be included from sbfBuffer.h"
#endif

struct sbfBufferImpl
{
    int                mAllocated;
    void*              mData;
    size_t             mSize;

    sbfBufferDestroyCb mDestroyCb;
    void*              mDestroyData;
    void*              mDestroyClosure;

    sbfRefCount        mRefCount;
    int                mLocked;

    void*              mOwner;
    void*              mHandlerData;
};

extern sbfPool gSbfBufferDefaultPool;

static SBF_INLINE sbfBuffer
sbfBuffer_new (sbfPool pool, size_t size)
{
    sbfBuffer buffer;

    if (pool == NULL)
    {
#ifdef WIN32
        buffer = (sbfBuffer)LocalAlloc (0, (sizeof *buffer) + size);
#else
        buffer = (sbfBuffer)xmalloc ((sizeof *buffer) + size);
#endif
        buffer->mAllocated = 1;
    }
    else
    {
        buffer = (sbfBuffer)sbfPool_get (pool);
        buffer->mAllocated = 0;
    }

    buffer->mDestroyCb = NULL;
    buffer->mDestroyData = NULL;
    buffer->mDestroyClosure = NULL;

    buffer->mData = buffer + 1;
    buffer->mSize = size;

    sbfRefCount_init (&buffer->mRefCount, 1);
    buffer->mLocked = 0;

    buffer->mOwner = NULL;

    return buffer;
}

static SBF_INLINE sbfBuffer
sbfBuffer_newZero (sbfPool pool, size_t size)
{
    sbfBuffer buffer;

    buffer = sbfBuffer_new (pool, size);
    memset (buffer->mData, 0, size);

    return buffer;
}

static SBF_INLINE sbfBuffer
sbfBuffer_copy (sbfPool pool, void* data, size_t size)
{
    sbfBuffer buffer;

    buffer = sbfBuffer_new (pool, size);
    memcpy (buffer->mData, data, size);

    return buffer;
}

static SBF_INLINE sbfBuffer
sbfBuffer_wrap (void* data, size_t size, sbfBufferDestroyCb cb, void* closure)
{
    sbfBuffer buffer;

    if (gSbfBufferDefaultPool == NULL)
    {
        gSbfBufferDefaultPool = sbfPool_create (sizeof (struct sbfBufferImpl));
        sbfPool_registerAtExit (gSbfBufferDefaultPool);
    }

    buffer = (sbfBuffer)sbfPool_get (gSbfBufferDefaultPool);
    buffer->mAllocated = 0;

    buffer->mDestroyCb = cb;
    buffer->mDestroyData = data;
    buffer->mDestroyClosure = closure;

    buffer->mData = data;
    buffer->mSize = size;

    sbfRefCount_init (&buffer->mRefCount, 1);
    buffer->mLocked = 0;

    return buffer;
}

static SBF_INLINE void
sbfBuffer_addRef (sbfBuffer buffer)
{
    sbfRefCount_increment (&buffer->mRefCount);
}

static SBF_INLINE void
sbfBuffer_lock (sbfBuffer buffer)
{
    buffer->mLocked = 1;
}

static SBF_INLINE void
sbfBuffer_destroy (sbfBuffer buffer)
{
    if (!sbfRefCount_decrement (&buffer->mRefCount))
        return;

    if (buffer->mDestroyCb != NULL)
    {
        buffer->mDestroyCb (buffer,
                            buffer->mDestroyData,
                            buffer->mDestroyClosure);
    }

    if (buffer->mAllocated)
#ifdef WIN32
        LocalFree (buffer);
#else
        free (buffer);
#endif
    else
        sbfPool_put (buffer);
}

static SBF_INLINE void*
sbfBuffer_getData (sbfBuffer buffer)
{
    return buffer->mData;
}

static SBF_INLINE void
sbfBuffer_setData (sbfBuffer buffer, void* data)
{
    if (SBF_UNLIKELY (buffer->mLocked))
        SBF_FATAL ("data set on locked buffer");
    buffer->mData = data;
}

static SBF_INLINE size_t
sbfBuffer_getSize (sbfBuffer buffer)
{
    return buffer->mSize;
}

static SBF_INLINE void
sbfBuffer_setSize (sbfBuffer buffer, size_t size)
{
    if (SBF_UNLIKELY (buffer->mLocked))
        SBF_FATAL ("size set on locked buffer");
    buffer->mSize = size;
}
