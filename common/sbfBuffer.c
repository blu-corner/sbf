#include "sbfBuffer.h"

sbfPool gSbfBufferDefaultPool;

sbfPool
sbfBuffer_createPool (size_t size)
{
    return sbfPool_create (sizeof (struct sbfBufferImpl) + size);
}
