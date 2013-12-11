#include "sbfUdpMulticast.h"
#include "sbfUdpMulticastPrivate.h"

sbfUdpMulticast
sbfUdpMulticast_create (sbfUdpMulticastType type,
                        in_addr_t interface,
                        struct sockaddr_in* address)
{
    sbfUdpMulticast s;
    int             n = 1;
    struct in_addr  ia;
    struct ip_mreq  im;

    s = xcalloc (1, sizeof *s);
    s->mType = type;
    s->mPool = sbfBuffer_createPool (SBF_UDP_MULTICAST_SIZE_LIMIT);

    memcpy (&s->mAddress, address, sizeof s->mAddress);
    s->mAddress.sin_family = AF_INET;

    s->mSocket = socket (AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (s->mSocket < 0)
        goto fail;

    if (setsockopt (s->mSocket, SOL_SOCKET, SO_REUSEADDR, &n, sizeof n) < 0)
        goto fail;

    if (bind (s->mSocket,
              (struct sockaddr*)&s->mAddress,
              sizeof s->mAddress) < 0)
        goto fail;

    if (type == SBF_UDP_MULTICAST_SEND)
    {
        memset (&ia, 0, sizeof ia);
        ia.s_addr = interface;
        if (setsockopt (s->mSocket,
                        IPPROTO_IP,
                        IP_MULTICAST_IF,
                        &ia,
                        sizeof ia) < 0)
            goto fail;
    }
    else
    {
        memset (&im, 0, sizeof im);
        im.imr_multiaddr.s_addr = address->sin_addr.s_addr;
        im.imr_interface.s_addr = interface;
        if (setsockopt (s->mSocket,
                        IPPROTO_IP,
                        IP_ADD_MEMBERSHIP,
                        &im,
                        sizeof im) < 0)
            goto fail;
    }

    return s;

fail:
    sbfUdpMulticast_destroy (s);
    return NULL;
}

void
sbfUdpMulticast_destroy (sbfUdpMulticast s)
{
    if (s->mSocket != -1)
        sbfCloseSocket (s->mSocket);

    sbfPool_destroy (s->mPool);
    free (s);
}

int
sbfUdpMulticast_getSocket (sbfUdpMulticast s)
{
    return s->mSocket;
}

sbfError
sbfUdpMulticast_send (sbfUdpMulticast s, const void* buf, size_t len)
{
    ssize_t sent;

    sent = sendto (s->mSocket,
                   buf,
                   len,
                   MSG_DONTWAIT,
                   (struct sockaddr*)&s->mAddress,
                   sizeof s->mAddress);
    if (sent < 0)
        return errno;
    if ((size_t)sent != len)
        return EIO;
    return 0;
}

sbfError
sbfUdpMulticast_read (sbfUdpMulticast s,
                      sbfUdpMulticastReadCb cb,
                      void* closure,
                      u_int limit)
{
    sbfBuffer buffer;
    sbfError  error;
    u_int     i;
    int       used;

    error = 0;
    for (i = 0; limit == 0 || i < limit; i++)
    {
        buffer = sbfBuffer_new (s->mPool, SBF_UDP_MULTICAST_SIZE_LIMIT);

        used = recv (s->mSocket,
                     sbfBuffer_getData (buffer),
                     sbfBuffer_getSize (buffer),
                     MSG_DONTWAIT);
        if (used == 0)
            break;
        if (used == -1)
        {
            if (errno == EINTR || errno == EAGAIN)
                break;
            error = errno;
            break;
        }

        sbfBuffer_setSize (buffer, used);
        error = cb (s, buffer, closure);
        if (error != 0)
            break;

        sbfBuffer_destroy (buffer);
    }

    if (i != 0)
        sbfBuffer_destroy (buffer);
    return error;
}

