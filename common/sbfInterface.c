#include "sbfInterface.h"

typedef struct
{
    const char* mName;
    uint32_t    mAddress;
} sbfInterface;

static int           gSbfInterfacesReady;
static u_int         gSbfInterfacesSize;
static sbfInterface* gSbfInterfaces;

static void
sbfInterfaceLog (sbfInterface* iff)
{
    char tmp[INET_ADDRSTRLEN];

    inet_ntop (AF_INET, &iff->mAddress, tmp, sizeof tmp);
    sbfLog_debug ("interface %s is %s", iff->mName, tmp);
}

static void
sbfInterfaceBuild (void)
{
#ifdef WIN32
    sbfInterface*         iff;
    IP_ADAPTER_ADDRESSES* aa0 = NULL;
    IP_ADAPTER_ADDRESSES* aa;
    ULONG                 needed;
    DWORD                 error;
    struct sockaddr_in*   sin;

    error = GetAdaptersAddresses (AF_INET, 0, NULL, aa0, &needed);
    if (error == ERROR_BUFFER_OVERFLOW)
        aa0 = xmalloc (needed);
    else if (error != NO_ERROR)
        return;

    error = GetAdaptersAddresses (AF_INET, 0, NULL, aa0, &needed);
    if (error != NO_ERROR)
    {
        free (aa0);
        return;
    }

    for (aa = aa0; aa != NULL; aa = aa->Next)
    {
        gSbfInterfaces = xrealloc (gSbfInterfaces,
                                   gSbfInterfacesSize + 1,
                                   sizeof *gSbfInterfaces);
        iff = &gSbfInterfaces[gSbfInterfacesSize++];

        sin = (struct sockaddr_in*)aa->FirstUnicastAddress->Address.lpSockaddr;

        iff->mName = xstrdup (aa->AdapterName);
        iff->mAddress = sin->sin_addr.s_addr;
        sbfInterfaceLog (iff);
    }

    free (aa);
#else
    sbfInterface*       iff;
    struct ifaddrs*     ifa0;
    struct ifaddrs*     ifa;
    struct sockaddr_in* sin;

    if (getifaddrs (&ifa0) != 0)
        return;

    for (ifa = ifa0; ifa != NULL; ifa = ifa->ifa_next)
    {
        if (ifa->ifa_addr == NULL)
            continue;
        sin = (struct sockaddr_in*)ifa->ifa_addr;
        if (sin->sin_family != AF_INET)
            continue;

        gSbfInterfaces = xrealloc (gSbfInterfaces,
                                   gSbfInterfacesSize + 1,
                                   sizeof *gSbfInterfaces);
        iff = &gSbfInterfaces[gSbfInterfacesSize++];

        iff->mName = xstrdup (ifa->ifa_name);
        iff->mAddress = sin->sin_addr.s_addr;
        sbfInterfaceLog (iff);
    }

    freeifaddrs (ifa0);
#endif
}

uint32_t
sbfInterface_find (const char* name)
{
    sbfInterface* iff;
    uint32_t      mask;
    u_int         i;
    uint16_t      octets0[4];
    union
    {
        uint32_t address;
        uint8_t  octets[4];
    } u = { 0 };

    if (!gSbfInterfacesReady)
    {
        sbfInterfaceBuild ();
        gSbfInterfacesReady = 1;
    }

    if (sscanf (name,
                "%hu.%hu.%hu.%hu",
                &octets0[0],
                &octets0[1],
                &octets0[2],
                &octets0[3]) == 4 &&
        octets0[0] < 256 &&
        octets0[1] < 256 &&
        octets0[2] < 256 &&
        octets0[3] < 256)
    {
        mask = htonl (0xffffffffU);
        u.octets[0] = (uint8_t)octets0[0];
        u.octets[1] = (uint8_t)octets0[1];
        u.octets[2] = (uint8_t)octets0[2];
        u.octets[3] = (uint8_t)octets0[3];
    }
    else if (sscanf (name,
                     "%hu.%hu.%hu",
                     &octets0[0],
                     &octets0[1],
                     &octets0[2]) == 3 &&
             octets0[0] < 256 &&
             octets0[1] < 256 &&
             octets0[2] < 256)
    {
        mask = htonl (0xffffff00U);
        u.octets[0] = (uint8_t)octets0[0];
        u.octets[1] = (uint8_t)octets0[1];
        u.octets[2] = (uint8_t)octets0[2];
    }
    else if (sscanf (name,
                     "%hu.%hu",
                     &octets0[0],
                     &octets0[1]) == 2 &&
             octets0[0] < 256 &&
             octets0[1] < 256)
    {
        mask = htonl (0xffff0000U);
        u.octets[0] = (uint8_t)octets0[0];
        u.octets[1] = (uint8_t)octets0[1];
    }
    else if (sscanf (name,
                     "%hu",
                     &octets0[0]) == 1 &&
             octets0[0] < 256)
    {
        mask = htonl (0xff000000U);
        u.octets[0] = (uint8_t)octets0[0];
    }
    else
        mask = 0;
    if (mask != 0)
    {
        for (i = 0; i < gSbfInterfacesSize; i++)
        {
            iff = &gSbfInterfaces[i];
            if ((iff->mAddress & mask) == (u.address & mask))
                return iff->mAddress;
        }
    }

    for (i = 0; i < gSbfInterfacesSize; i++)
    {
        iff = &gSbfInterfaces[i];
        if (strcmp (iff->mName, name) == 0)
            return iff->mAddress;
    }
    return 0;
}

sbfError
sbfInterface_getHostPort (const char* s, struct sockaddr_in* sin)
{
    char*         copy;
    char*         cp;
    char*         endptr;
    unsigned long ul;

    copy = xstrdup (s);

    cp = strchr (copy, ':');
    if (cp == NULL)
        return EINVAL;

    ul = strtoul (cp + 1, &endptr, 10);
    if (ul == 0 || ul > UINT16_MAX || *endptr != '\0')
        return ERANGE;
    sin->sin_port = ntohs ((u_short)ul);

    *cp = '\0';
    if (inet_pton (AF_INET, copy, &sin->sin_addr) == 0)
        return EINVAL;
    sin->sin_family = AF_INET;

    free (copy);
    return 0;
}
