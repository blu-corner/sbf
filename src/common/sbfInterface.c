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
sbfInterfaceLog (sbfLog log, sbfInterface* iff)
{
    char tmp[INET_ADDRSTRLEN];

    inet_ntop (AF_INET, &iff->mAddress, tmp, sizeof tmp);
    sbfLog_debug (log, "interface %s is %s", iff->mName, tmp);
}

static void
sbfInterfaceAtExit (void)
{
    u_int i;

    for (i = 0; i < gSbfInterfacesSize; i++)
        free ((void*)gSbfInterfaces[i].mName);
    free (gSbfInterfaces);
}

static void
sbfInterfaceBuild (sbfLog log)
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
        sbfInterfaceLog (log, iff);
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
        sbfInterfaceLog (log, iff);
    }

    freeifaddrs (ifa0);
#endif

    atexit (sbfInterfaceAtExit);
}

static uint32_t
sbfInterfaceFind (sbfLog log, const char* name, sbfInterface** iffp)
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
        sbfInterfaceBuild (log);
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
            {
                if (iffp != NULL)
                    *iffp = iff;
                return iff->mAddress;
            }
        }
    }

    for (i = 0; i < gSbfInterfacesSize; i++)
    {
        iff = &gSbfInterfaces[i];
        if (strcmp (iff->mName, name) == 0)
        {
            if (iffp != NULL)
                *iffp = iff;
            return iff->mAddress;
        }
    }
    return 0;
}

uint32_t
sbfInterface_first (sbfLog log)
{
    sbfInterface* iff;
    u_int         i;

    if (!gSbfInterfacesReady)
    {
        sbfInterfaceBuild (log);
        gSbfInterfacesReady = 1;
    }

    for (i = 0; i < gSbfInterfacesSize; i++)
    {
        iff = &gSbfInterfaces[i];
        if (strcmp (iff->mName, "lo") == 0)
            continue;
        if ((ntohl (iff->mAddress) & 0xff000000U) == 127)
            continue;
        return iff->mAddress;
    }
    return 0;
}

uint32_t
sbfInterface_find (sbfLog log, const char* name)
{
    return sbfInterfaceFind (log, name, NULL);
}

uint32_t
sbfInterface_findOne (sbfLog log, const char* names, const char** name)
{
    char*         copy;
    char*         cp;
    char*         next;
    uint32_t      address;
    sbfInterface* iff;

    cp = copy = xstrdup (names);
    while ((next = strsep (&cp, ",")) != NULL)
    {
        address = sbfInterfaceFind (log, next, &iff);
        if (address != 0)
        {
            if (name != NULL)
                *name = iff->mName;
            free (copy);
            return address;
        }
    }
    free (copy);
    return 0;
}

sbfError
sbfInterface_parseAddress (const char* s, struct sockaddr_in* sin)
{
    char*         copy;
    char*         cp;
    char*         endptr;
    unsigned long ul;

    copy = xstrdup (s);

    cp = strchr (copy, ':');
    if (cp == NULL || cp[1] == '\0')
    {
        free (copy);
        return EINVAL;
    }

    ul = strtoul (cp + 1, &endptr, 10);
    if (ul == 0 || ul > UINT16_MAX || *endptr != '\0')
    {
        free (copy);
        return ERANGE;
    }
    sin->sin_port = ntohs ((u_short)ul);

    *cp = '\0';
    if (inet_pton (AF_INET, copy, &sin->sin_addr) == 0)
    {
        free (copy);
        return EINVAL;
    }
    sin->sin_family = AF_INET;

    free (copy);
    return 0;
}

sbfError
sbfInterface_resolveAddress (const char* s,
                             struct sockaddr_in* sin,
                             const char** error)
{
    char*            copy;
    char*            cp;
    struct addrinfo  hints, *res, *res0;
    int              retval;
    struct sockaddr* sa;

    copy = xstrdup (s);

    cp = strchr (copy, ':');
    if (cp == NULL || cp[1] == '\0')
    {
        free (copy);
        if (error != NULL)
            *error = strerror (EINVAL);
        return EINVAL;
    }
    *cp++ = '\0';

    memset (&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = 0;
    hints.ai_protocol = 0;

    retval = evutil_getaddrinfo (copy, cp, &hints, &res0);
    if (retval != 0)
    {
        if (error != NULL)
            *error = gai_strerror (retval);
        return EINVAL;
    }
    for (res = res0; res != NULL; res = res->ai_next)
    {
        if (res->ai_addrlen != sizeof (struct sockaddr_in))
            continue;
        sa = res->ai_addr;
        if (sa->sa_family != AF_INET)
            continue;
        break;
    }
    if (res == NULL)
    {
        free (copy);
        if (error != NULL)
            *error = strerror (EADDRNOTAVAIL);
        return EADDRNOTAVAIL;
    }
    memcpy (sin, res->ai_addr, sizeof *sin);
    freeaddrinfo (res0);

    free (copy);
    if (error != NULL)
            *error = NULL;
    return 0;
}
