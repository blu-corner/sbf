#include "sbfBuffer.h"

sbfPool gSbfBufferDefaultPool;

sbfPool
sbfBuffer_createPool1 (size_t size, const char* function, u_int line)
{
    return sbfPool_create1 (sizeof (struct sbfBufferImpl) + size,
                            function,
                            line);
}

sbfPool
sbfBuffer_defaultPool (size_t size)
{
    return sbfPool_default (sizeof (struct sbfBufferImpl) + size);
}
