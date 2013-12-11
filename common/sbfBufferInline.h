#ifndef _SBF_BUFFER_H_
#error "must be included from sbfBuffer.h"
#endif

struct sbfBufferImpl
{
    int                mAllocated;
    void*              mData;
    size_t             mSize;

    sbfBufferDestroyCb mDestroyCb;
    void*              mDestroyClosure;

    sbfRefCount        mRefCount;

    void*              mOwner;
};

extern sbfPool gSbfBufferDefaultPool;

static SBF_INLINE sbfBuffer
sbfBuffer_new (sbfPool pool, size_t size)
{
    sbfBuffer buffer;

    if (pool == NULL)
    {
        buffer = (sbfBuffer)xmalloc ((sizeof *buffer) + size);
        buffer->mAllocated = 1;
    }
    else
    {
        buffer = (sbfBuffer)sbfPool_get (pool);
        buffer->mAllocated = 0;
    }

    buffer->mDestroyCb = NULL;
    buffer->mDestroyClosure = NULL;

    buffer->mData = buffer + 1;
    buffer->mSize = size;

    sbfRefCount_init (&buffer->mRefCount, 1);

    buffer->mOwner = NULL;

    return buffer;
}

static SBF_INLINE sbfBuffer
sbfBuffer_newZero (sbfPool pool, size_t size)
{
    sbfBuffer buffer;

    buffer = sbfBuffer_new (pool, size);
    memset (sbfBuffer_getData (buffer), 0, sbfBuffer_getSize (buffer));

    return buffer;
}

static SBF_INLINE sbfBuffer
sbfBuffer_wrap (void* data,
                size_t size,
                sbfBufferDestroyCb cb,
                void* closure)
{
    sbfBuffer buffer;

    if (gSbfBufferDefaultPool == NULL)
        gSbfBufferDefaultPool = sbfPool_create (sizeof (struct sbfBufferImpl));

    buffer = (sbfBuffer)sbfPool_get (gSbfBufferDefaultPool);
    buffer->mAllocated = 0;

    buffer->mDestroyCb = cb;
    buffer->mDestroyClosure = closure;

    buffer->mData = data;
    buffer->mSize = size;

    sbfRefCount_init (&buffer->mRefCount, 1);

    return buffer;
}

static SBF_INLINE void
sbfBuffer_addRef (sbfBuffer buffer)
{
    sbfRefCount_increment (&buffer->mRefCount);
}

static SBF_INLINE void
sbfBuffer_destroy (sbfBuffer buffer)
{
    if (!sbfRefCount_decrement (&buffer->mRefCount))
        return;

    if (buffer->mDestroyCb != NULL)
        buffer->mDestroyCb (buffer, buffer->mDestroyClosure);

    if (buffer->mAllocated)
        free (buffer);
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
    buffer->mSize = size;
}
