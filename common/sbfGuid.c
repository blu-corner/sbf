#include "sbfGuid.h"
#include "sbfInterface.h"

void
sbfGuid_new (sbfGuid* g, uint64_t seed)
{
    struct timeval  tv;
    uint32_t        pid;
    uint32_t        host;

    gettimeofday (&tv, NULL);
    pid = getpid ();

    host = sbfInterface_first ();
    if (host == 0)
        SBF_FATAL ("can't find machine host");

    g->mParts[0] = ntohl (host);
    g->mParts[1] = ((uint64_t)tv.tv_sec << 32) | pid;
    g->mParts[2] = seed;
    g->mParts[3] = 0;
}

int
sbfGuid_compare (const sbfGuid* g1, const sbfGuid* g2)
{
    return memcmp (g1, g2, sizeof *g1);
}

void
sbfGuid_copy (sbfGuid* to, const sbfGuid* from)
{
    memcpy (to, from, sizeof *to);
}

sbfGuid*
sbfGuid_increment (sbfGuid* g, u_int n)
{
    g->mParts[3] += n;
    return g;
}

char*
sbfGuid_toString (const sbfGuid* g, char* buf, size_t len)
{
    snprintf (buf,
              len,
              "%016llx/%016llx/%016llx/%016llx",
              (unsigned long long)g->mParts[0],
              (unsigned long long)g->mParts[1],
              (unsigned long long)g->mParts[2],
              (unsigned long long)g->mParts[3]);
    return buf;
}
