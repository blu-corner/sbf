#include "sbfCommon.h"

int
sbfMutex_init (pthread_mutex_t* m, int recursive)
{
    pthread_mutexattr_t a;
    int                 retval;

    pthread_mutexattr_init (&a);
    if (recursive)
        pthread_mutexattr_settype (&a, PTHREAD_MUTEX_RECURSIVE);

    retval = pthread_mutex_init (m, &a);

    pthread_mutexattr_destroy (&a);

    return retval;
}

