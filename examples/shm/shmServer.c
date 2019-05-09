#include "sbfSharedMemoryRingBuffer.h"
#include "stdbool.h"

int main (int argc, char** argv)
{
    const char* errorText = NULL;
    sbfShmMemoryRingBuffer buffer = sbfShmMemoryRingBuffer_create ("./shm1.dat",
                                                                   5,  // element size
                                                                   10, // capacity
                                                                   &errorText);
    if (buffer == NULL) {
        fprintf (stderr, "failed to create ring-buffer: %s\n", errorText);
        return -1;
    }

    const char* data = "cake";
    bool ok = sbfShmMemoryRingBuffer_enqueue (buffer, (const void*)data, 4) == 0;
    if (!ok) {
        fprintf (stderr, "failed to insert");
        return -1;
    }

    sbfShmMemoryRingBuffer_destroy (buffer);
    
    return 0;
}
