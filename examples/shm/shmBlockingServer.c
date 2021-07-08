#include "sbfShmRingBuffer.h"
#include "stdbool.h"


int main (int argc, char** argv)
{
    const char* errorText = NULL;
    sbfShmRingBuffer buffer = sbfShmRingBuffer_create ("./shm1.dat",
                                                       6,  // element size
                                                       5,  // capacity
                                                       &errorText);
    if (buffer == NULL) {
        fprintf (stderr, "failed to create ring-buffer: %s\n", errorText);
        return -1;
    }

    char data[6] = {0};
    
    bool ok;
    char i;
    for (i = 0; i < 5; ++i) {
        snprintf (data, sizeof data, "cake%i", i);
        printf("ENQUEUE: [%i] - [%s]\n", i, data);
        
        ok = sbfShmRingBuffer_blockingEnqueue (buffer, (const void*)data, sizeof data) == 0;
        if (!ok) {
            fprintf (stderr, "failed to insert");
            return -1;
        }
    }

    // this one should block
    printf("BLOCKING OVER CAPACITY INSERT\n");
    
    snprintf (data, sizeof data, "cake%i", i);
    printf("ENQUEUE: [%i] - [%s]\n", i, data);
    
    ok = sbfShmRingBuffer_blockingEnqueue (buffer, (const void*)data, sizeof data) == 0;
    if (!ok) {
        fprintf (stderr, "failed to insert");
        return -1;
    }

    printf("BLOCKING OVER CAPACITY INSERT - Done\n");

    for (i = 0; i < 4; ++i) {
        snprintf (data, sizeof data, "cake%i", i);
        printf("ENQUEUE: [%i] - [%s]\n", i, data);
        
        ok = sbfShmRingBuffer_blockingEnqueue (buffer, (const void*)data, sizeof data) == 0;
        if (!ok) {
            fprintf (stderr, "failed to insert");
            return -1;
        }
    }

    sbfShmRingBuffer_destroy (buffer);
    
    return 0;
}
