#include "sbfTport.h"
#include "sbfTportPrivate.h"

void
sbfTport_parseWeights (sbfTport tport)
{
    sbfKeyValue      properties = tport->mProperties;
    u_int            i;
    char             name[64];
    const char*      value;
    const char*      cp;
    char             pattern[256];
    unsigned long    ul;
    char*            endptr;
    sbfTportWeight*  w;

    for (i = 0; i < USHRT_MAX; i++)
    {
        snprintf (name, sizeof name, "weight%u", i);
        if ((value = sbfKeyValue_get (properties, name)) == NULL)
            continue;
        cp = strchr (value, ',');
        if (cp == NULL)
        {
            sbfLog_warn (tport->mLog,
                         "missing comma in weight%u property (%s)",
                         i,
                         value);
            continue;
        }

        ul = strtoul (cp + 1, &endptr, 10);
        if (ul > USHRT_MAX || (endptr != NULL && *endptr != '\0'))
        {
            sbfLog_warn (tport->mLog,
                         "bad number in weight%u property (%s)",
                         i,
                         value);
            continue;
        }

        if ((size_t)(cp - value) > (sizeof pattern) - 1)
        {
            sbfLog_warn (tport->mLog,
                         "weight%u pattern is too long (%s)",
                         i,
                         value);
            continue;
        }
        memcpy (pattern, value, cp - value);
        pattern[cp - value] = '\0';

        tport->mWeightsList = xrealloc (tport->mWeightsList,
                                        tport->mWeightsListSize + 1,
                                        sizeof *tport->mWeightsList);
        w = &tport->mWeightsList[tport->mWeightsListSize++];

        if (regcomp (&w->mPattern, pattern, REG_EXTENDED|REG_NOSUB) != 0)
        {
            sbfLog_warn (tport->mLog,
                         "weight%u pattern is invalid (%s)",
                         i,
                         value);
            tport->mWeightsListSize--;
            continue;
        }
        w->mWeight = ul;
        sbfLog_debug (tport->mLog,
                      "weight pattern %s is %u",
                      pattern,
                      w->mWeight);
    }

}

u_int
sbfTport_topicWeight (sbfTport tport, sbfTopic topic)
{
    sbfTportWeight* w;
    u_int           i;

    for (i = 0; i < tport->mWeightsListSize; i++)
    {
        w = &tport->mWeightsList[i];
        if (regexec (&w->mPattern, sbfTopic_getTopic (topic), 0, NULL, 0) == 0)
            return w->mWeight;
    }
    return 1;
}

void
sbfTport_adjustWeight (sbfTport tport, sbfMwThread thread, int change)
{
    u_int index;

    sbfMutex_lock (&tport->mWeightsLock);

    index = sbfMw_getThreadIndex (thread);
    tport->mWeights[index] += change;

    sbfLog_debug (tport->mLog,
                  "thread %u weight is %u (%+d)",
                  index,
                  tport->mWeights[index],
                  change);

    sbfMutex_unlock (&tport->mWeightsLock);
}

sbfMwThread
sbfTport_nextThread (sbfTport tport)
{
    sbfMwThread thread;
    sbfMwThread found;
    u_int       weight;
    u_int       i;

    sbfMutex_lock (&tport->mWeightsLock);

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

    sbfMutex_unlock (&tport->mWeightsLock);
    return found;
}
