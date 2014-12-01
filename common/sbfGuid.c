#include "sbfGuid.h"
#include "sbfInterface.h"

static uint64_t gSbfGuidSeed;

void
sbfGuid_new (sbfLog log, sbfGuid* g)
{
    struct timeval  tv;
    uint32_t        pid;
    uint32_t        host;

    gettimeofday (&tv, NULL);
    pid = getpid ();

    host = sbfInterface_first (log);
    if (host == 0)
        SBF_FATAL ("can't find machine host");

    g->mParts[0] = ntohl (host);
    g->mParts[1] = ((uint64_t)tv.tv_sec << 32) | pid;
    g->mParts[2] = gSbfGuidSeed++;
    g->mParts[3] = 0;
}

int
sbfGuid_compare (const sbfGuid* g1, const sbfGuid* g2)
{
    return memcmp (g1, g2, sizeof *g1);
}

int
sbfGuid_compareFixed (const sbfGuid* g1, const sbfGuid* g2)
{
    return memcmp (g1, g2, (sizeof *g1) - sizeof g1->mParts[3]);
}

void
sbfGuid_copy (sbfGuid* to, const sbfGuid* from)
{
    memcpy (to, from, sizeof *to);
}

uint64_t
sbfGuid_get (sbfGuid* g)
{
    return g->mParts[3];
}

void
sbfGuid_set (sbfGuid* g, uint64_t to)
{
    g->mParts[3] = to;
}

uint64_t
sbfGuid_increment (sbfGuid* g, uint64_t by)
{
    g->mParts[3] += by;
    return g->mParts[3];
}

int
sbfGuid_fromString (sbfGuid* g, const char* s)
{
    if (sscanf (s,
                "%llx/%llx/%llx/%llx",
                (unsigned long long*)&g->mParts[0],
                (unsigned long long*)&g->mParts[1],
                (unsigned long long*)&g->mParts[2],
                (unsigned long long*)&g->mParts[3]) != 4)
        return 0;
    return 1;
}

const char*
sbfGuid_toString (const sbfGuid* g)
{
    static __thread char s[SBF_GUID_STRING_SIZE];

    return sbfGuid_toStringBuffer (g, s, sizeof s);
}

char*
sbfGuid_toStringBuffer (const sbfGuid* g, char* buf, size_t len)
{
    snprintf (buf,
              len,
              "%016llx/%016llx/%llx/%llx",
              (unsigned long long)g->mParts[0],
              (unsigned long long)g->mParts[1],
              (unsigned long long)g->mParts[2],
              (unsigned long long)g->mParts[3]);
    return buf;
}

const char*
sbfGuid_toStringFixed (const sbfGuid* g)
{
    static __thread char s[SBF_GUID_STRING_SIZE];

    return sbfGuid_toStringFixedBuffer (g, s, sizeof s);
}

char*
sbfGuid_toStringFixedBuffer (const sbfGuid* g, char* buf, size_t len)
{
    snprintf (buf,
              len,
              "%016llx/%016llx/%llx",
              (unsigned long long)g->mParts[0],
              (unsigned long long)g->mParts[1],
              (unsigned long long)g->mParts[2]);
    return buf;
}
