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
        free (aa);
        return;
    }

    aa = aa;
    for (aa = aa0; aa != NULL; aa = aa->Next)
    {
        gSbfInterfaces = xrealloc (gSbfInterfaces,
                                   gSbfInterfacesSize + 1,
                                   sizeof *gSbfInterfaces);
        iff = &gSbfInterfaces[gSbfInterfacesSize++];

        sin = (struct sockaddr_in*)aa->FirstUnicastAddress->Address.lpSockaddr;

        iff->mName = xstrdup (aa->AdapterName);
        iff->mAddress = sin->sin_addr.s_addr;
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
                "%hhu.%hhu.%hhu.%hhu",
                &u.octets[0],
                &u.octets[1],
                &u.octets[2],
                &u.octets[3]) == 4)
        mask = htonl (0xffffffffU);
    else if (sscanf (name,
                     "%hhu.%hhu.%hhu",
                     &u.octets[0],
                     &u.octets[1],
                     &u.octets[2]) == 3)
        mask = htonl (0xffffff00U);
    else if (sscanf (name, "%hhu.%hhu", &u.octets[0], &u.octets[1]) == 2)
        mask = htonl (0xffff0000U);
    else if (sscanf (name, "%hhu", &u.octets[0]) == 1)
        mask = htonl (0xff000000U);
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
