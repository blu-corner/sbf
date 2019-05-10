#include "sbfShmRingBuffer.h"
#include "stdbool.h"


int main (int argc, char** argv)
{
    const char* errorText = NULL;
    sbfShmRingBuffer buffer = sbfShmRingBuffer_attach ("./shm1.dat",
                                                       6,  // element size
                                                       5,  // capacity
                                                       &errorText);
    if (buffer == NULL) {
        fprintf (stderr, "failed to attach ring-buffer: %s\n", errorText);
        return -1;
    }

    void* data;
    size_t length;
    bool ok = sbfShmRingBuffer_dequeueAllocated (buffer,
                                                 &data,
                                                 &length) == 0;
    if (!ok) {
        fprintf (stderr, "failed to dequeue\n");
        return -1;
    }

    char buf[length + 1];
    memset (buf, 0, sizeof (buf));
    memcpy (buf, data, length);
    
    printf("dequeued: [%s]\n", buf);

    sbfShmRingBuffer_destroy (buffer);
    free(data);
    
    return 0;
}
