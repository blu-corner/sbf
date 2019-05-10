#include "SbfSharedMemory.hpp"

int main (int argc, char** argv)
{
    std::string error;
    neueda::SbfShmRingBuffer* buffer =
        neueda::SbfShmRingBuffer::create ("./shmcpp.dat",
                                          5,
                                          5,
                                          error);
    if (buffer == nullptr) {
        fprintf (stderr, "failed to create shm buffer: [%s]\n", error.c_str ());
        return -1;
    }
    
    std::string cake = "cake";
    buffer->enqueue ((const void*)cake.c_str(), cake.length ());

    char* buf;
    size_t length;
    buffer->blockingAllocatedDequeue ((void**)&buf, &length);

    printf ("[%s]\n", buf);
    free (buf);

    delete buffer;
    
    return 0;
}
