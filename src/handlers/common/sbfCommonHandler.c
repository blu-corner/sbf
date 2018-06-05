#include "sbfCommonHandler.h"
#include "sbfCommonHandlerPrivate.h"

static uint32_t
sbfMcastTopicResolverParseRange (const char* s, u_int* bits)
{
    uint16_t     octets0[4];
    union
    {
        uint32_t address;
        uint8_t  octets[4];
    } u = { 0 };

    *bits = 0;

    if (sscanf (s,
                "%hu.%hu.%hu.%hu/%u",
                &octets0[0],
                &octets0[1],
                &octets0[2],
                &octets0[3],
                bits) == 5 &&
        octets0[0] < 256 &&
        octets0[1] < 256 &&
        octets0[2] < 256 &&
        octets0[3] < 256)
    {
        u.octets[0] = (uint8_t)octets0[0];
        u.octets[1] = (uint8_t)octets0[1];
        u.octets[2] = (uint8_t)octets0[2];
        u.octets[3] = (uint8_t)octets0[3];
        return u.address;
    }

    if (sscanf (s,
                "%hu.%hu.%hu/%u",
                &octets0[0],
                &octets0[1],
                &octets0[2],
                bits) == 4 &&
        octets0[0] < 256 &&
        octets0[1] < 256 &&
        octets0[2] < 256)
    {
        u.octets[0] = (uint8_t)octets0[0];
        u.octets[1] = (uint8_t)octets0[1];
        u.octets[2] = (uint8_t)octets0[2];
        return u.address;
    }

    if (sscanf (s,
                "%hu.%hu/%u",
                &octets0[0],
                &octets0[1],
                bits) == 3 &&
        octets0[0] < 256 &&
        octets0[1] < 256)
    {
        u.octets[0] = (uint8_t)octets0[0];
        u.octets[1] = (uint8_t)octets0[1];
        return u.address;
    }

    if (sscanf (s,
                "%hu/%u",
                &octets0[0],
                bits) == 2 &&
        octets0[0] < 256)
    {
        u.octets[0] = (uint8_t)octets0[0];
        return u.address;
    }

    return 0;
}

sbfMcastTopicResolver
sbfMcastTopicResolver_create (const char* address,
                              sbfLog log)
{
    uint32_t              range;
    u_int                 bits;
    u_int                 first;
    u_int                 second;
    u_int                 third;
    char                  tmp[INET_ADDRSTRLEN];
    sbfMcastTopicResolver mtr;

    if (address == NULL)
        address = "239.100/16";
    range = sbfMcastTopicResolverParseRange (address, &bits);
    if (range == 0)
    {
        sbfLog_err (log, "couldn't get range: %s", address);
        return NULL;
    }
    if (bits > 24)
    {
        sbfLog_err (log, "need at least eight bits: %s", address);
        return NULL;
    }
    bits = 32 - bits;

    first = (bits / 6) * 1;
    second = (bits / 6) * 1;
    third = bits - first - second;

    mtr = xcalloc (1, sizeof *mtr);
    mtr->mLog = log;

    mtr->mBase = range;

    mtr->mFirstSize = 1 + ~(0xffffffffU << first);
    mtr->mSecondSize = 1 + ~(0xffffffffU << second);
    mtr->mThirdSize = 1 + ~(0xffffffffU << third) - 2;

    mtr->mFirstShift = second + third;
    mtr->mSecondShift = third;

    inet_ntop (AF_INET, &mtr->mBase, tmp, sizeof tmp);
    sbfLog_debug (mtr->mLog,
                  "range %s/%u: 1(%u, shift %u), 2(%u, shift %u), 3(%u)",
                  tmp,
                  bits,
                  mtr->mFirstSize,
                  mtr->mFirstShift,
                  mtr->mSecondSize,
                  mtr->mSecondShift,
                  mtr->mThirdSize);

    return mtr;
}

uint32_t
sbfMcastTopicResolver_makeAddress (sbfMcastTopicResolver mtr, sbfTopic topic)
{
    uint32_t h;
    uint32_t address;

    address = ntohl (mtr->mBase);

    h = hash32_str (sbfTopic_getFirst (topic), HASHINIT);
    address |= (h % mtr->mFirstSize) << mtr->mFirstShift;

    h = hash32_str (sbfTopic_getSecond (topic), HASHINIT);
    address |= (h % mtr->mSecondSize) << mtr->mSecondShift;

    h = hash32_str (sbfTopic_getThird (topic), HASHINIT);
    address |= 1 + (h % mtr->mThirdSize);

    return htonl (address);
}

void
sbfMcastTopicResolver_destroy (sbfMcastTopicResolver mtr)
{
    free (mtr);
}
