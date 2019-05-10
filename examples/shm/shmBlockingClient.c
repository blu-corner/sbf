#include "sbfShmRingBuffer.h"
#include "stdbool.h"

int main (int argc, char** argv)
{
    const char* errorText = NULL;
    sbfShmRingBuffer buffer = sbfShmRingBuffer_attach ("./shm1.dat",
                                                       5,  // element size
                                                       5,  // capacity
                                                       &errorText);
    if (buffer == NULL) {
        fprintf (stderr, "failed to attach ring-buffer: %s\n", errorText);
        return -1;
    }
    
    for (int i = 0; i < 10; ++ i) {
        char data[5] = { 0 };
        
        bool ok = sbfShmRingBuffer_blockingDequeue (buffer,
                                                    (void*)&data,
                                                    sizeof data) == 0;
        if (!ok) {
            fprintf (stderr, "failed to dequeue\n");
            return -1;
        }
        
        char buf[sizeof data + 1];
        memset(buf, 0, sizeof (buf));
        memcpy(buf, data, sizeof data);
    
        printf("dequeued: [%s]\n", buf);
    }

    sbfShmRingBuffer_destroy (buffer);
    
    return 0;
}
