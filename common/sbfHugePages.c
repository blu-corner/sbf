#include <sbfHugePages.h>

#ifndef MAP_HUGETLB
#define MAP_HUGETLB 0x40000
#endif

void*
sbfHugePages_allocate (size_t size)
{
    void* block;

    block = mmap (NULL,
                  size,
                  PROT_READ|PROT_WRITE,
                  MAP_PRIVATE|MAP_ANONYMOUS|MAP_HUGETLB,
                  -1,
                  0);
    if (block != MAP_FAILED)
        return block;
    sbfLog_warn ("can't use MAP_HUGETLB; check /proc/sys/vm/nr_hugepages");

    block = mmap (NULL,
                  size,
                  PROT_READ|PROT_WRITE,
                  MAP_PRIVATE|MAP_ANONYMOUS,
                  -1,
                  0);
    if (block != MAP_FAILED)
        return block;
    return NULL;
}

void
sbfHugePages_free (void* block, size_t size)
{
    munmap (block, size);
}
