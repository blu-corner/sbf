#include "sbfTport.h"
#include "sbfTportPrivate.h"

u_int
sbfTport_topicWeight (sbfTport tport, sbfTopic topic)
{
    return 1;
}

sbfMwThread
sbfTport_nextThread (sbfTport tport)
{
    sbfMwThread thread;
    sbfMwThread found;
    u_int       weight;
    u_int       i;

    pthread_mutex_lock (&tport->mWeightsLock);

    found = NULL;
    weight = UINT_MAX;

    for (i = 0; i < sbfMw_getNumThreads (tport->mMw); i++)
    {
        thread = sbfMw_getThread (tport->mMw, i);
        if (~tport->mThreads & sbfMw_getThreadMask (thread))
            continue;

        if (tport->mWeights[i] < weight)
        {
            found = thread;
            weight = tport->mWeights[i];
        }
    }

    pthread_mutex_unlock (&tport->mWeightsLock);
    return found;
}

