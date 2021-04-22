#include "SbfBuffer.hpp"

neueda::SbfBuffer::SbfBuffer (size_t size)
{
    mBuffer = sbfBuffer_newZero (NULL, size);
}

neueda::SbfBuffer::SbfBuffer (void* data, size_t size)
{
    mBuffer = sbfBuffer_copy (NULL, data, size);
}

neueda::SbfBuffer::~SbfBuffer ()
{
    if (getHandle () != NULL)
        sbfBuffer_destroy (getHandle ());
}

sbfBuffer neueda::SbfBuffer::getHandle ()
{
    return mBuffer;
}

void neueda::SbfBuffer::addRef ()
{
    sbfBuffer_addRef (getHandle ());
}

void neueda::SbfBuffer::lock ()
{
    sbfBuffer_lock (getHandle ());
}

void neueda::SbfBuffer::destroy ()
{
    sbfBuffer_destroy (getHandle ());
}

void* neueda::SbfBuffer::getData ()
{
    return sbfBuffer_getData (getHandle ());
}

void neueda::SbfBuffer::setData (void* data)
{
    sbfBuffer_setData (getHandle (), data);
}

size_t neueda::SbfBuffer::getSize ()
{
    return sbfBuffer_getSize (getHandle ());
}

void neueda::SbfBuffer::setSize (size_t size)
{
    sbfBuffer_setSize (getHandle (), size);
}
