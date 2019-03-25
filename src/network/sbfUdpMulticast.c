#include "sbfUdpMulticast.h"
#include "sbfUdpMulticastPrivate.h"

sbfUdpMulticast
sbfUdpMulticast_create (sbfUdpMulticastType type,
                        uint32_t interf,
                        struct sockaddr_in* address)
{
    sbfUdpMulticast    s;
    struct in_addr     ia;
    struct ip_mreq     im;
    struct sockaddr_in sin;
#ifdef WIN32
    char               opt = 1;
#else
    int                opt = 1;
#endif

    s = xcalloc (1, sizeof *s);
    s->mType = type;
    s->mPool = sbfBuffer_createPool (SBF_UDP_MULTICAST_SIZE_LIMIT);

    memcpy (&s->mAddress, address, sizeof s->mAddress);
    s->mAddress.sin_family = AF_INET;

    memset (&s->mReceiveAddress, 0x0, sizeof s->mReceiveAddress);

    s->mSocket = socket (AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (s->mSocket < 0)
        goto fail;
    evutil_make_socket_closeonexec (s->mSocket);
    evutil_make_socket_nonblocking (s->mSocket);

    if (type == SBF_UDP_MULTICAST_SEND)
    {
        memset (&ia, 0, sizeof ia);
        ia.s_addr = interf;
        if (setsockopt (s->mSocket,
                        IPPROTO_IP,
                        IP_MULTICAST_IF,
                        (void*)&ia,
                        sizeof ia) < 0)
            goto fail;
    }
    else
    {
        if (setsockopt (s->mSocket,
                        SOL_SOCKET,
                        SO_REUSEADDR,
                        &opt,
                        sizeof opt) < 0)
            goto fail;

        memset (&sin, 0, sizeof sin);
        sin.sin_family = AF_INET;
#ifdef WIN32
        sin.sin_addr.s_addr = INADDR_ANY;
#else
        sin.sin_addr.s_addr = address->sin_addr.s_addr;
#endif
        sin.sin_port = address->sin_port;
        if (bind (s->mSocket, (struct sockaddr*)&sin, sizeof sin) < 0)
            goto fail;

        if ((address->sin_addr.s_addr & 0xFF) >= 224)
        {
            memset (&im, 0, sizeof im);
            im.imr_multiaddr.s_addr = address->sin_addr.s_addr;
            im.imr_interface.s_addr = interf;
            if (setsockopt (s->mSocket,
                            IPPROTO_IP,
                            IP_ADD_MEMBERSHIP,
                            (void*)&im,
                            sizeof im) < 0)
                goto fail; 
        }
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
        EVUTIL_CLOSESOCKET (s->mSocket);

    sbfPool_destroy (s->mPool);
    free (s);
}

sbfSocket
sbfUdpMulticast_getSocket (sbfUdpMulticast s)
{
    return s->mSocket;
}

const struct sockaddr_in*
sbfUdpMulticast_getReceiveAddress (sbfUdpMulticast s)
{
    return &s->mReceiveAddress;
}

sbfError
sbfUdpMulticast_send (sbfUdpMulticast s, const void* buf, size_t len)
{
#ifdef WIN32
    int sent;
#else
    ssize_t sent;
#endif

    sent = sendto (s->mSocket,
                   buf,
                   len,
                   0,
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
    socklen_t sockLen = sizeof (s->mReceiveAddress);

    error = 0;
    buffer = NULL;

    for (i = 0; limit == 0 || i < limit; i++)
    {
        buffer = sbfBuffer_new (s->mPool, SBF_UDP_MULTICAST_SIZE_LIMIT);

        used = recvfrom (s->mSocket,
                         sbfBuffer_getData (buffer),
                         sbfBuffer_getSize (buffer),
                         0,
                         (struct sockaddr*)&s->mReceiveAddress,
                         &sockLen);
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
        buffer = NULL;
    }

    if (buffer != NULL)
        sbfBuffer_destroy (buffer);
    return error;
}
