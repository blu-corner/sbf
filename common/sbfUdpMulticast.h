#ifndef _SBF_UDP_MULTICAST_H_
#define _SBF_UDP_MULTICAST_H_

#include "sbfBuffer.h"
#include "sbfCommon.h"

SBF_BEGIN_DECLS

typedef struct sbfUdpMulticastImpl* sbfUdpMulticast;

#define SBF_UDP_MULTICAST_SIZE_LIMIT 65536

typedef enum
{
    SBF_UDP_MULTICAST_SEND,
    SBF_UDP_MULTICAST_LISTEN
} sbfUdpMulticastType;

typedef sbfError (*sbfUdpMulticastReadCb) (sbfUdpMulticast s,
                                           sbfBuffer buffer,
                                           void* closure);

sbfUdpMulticast sbfUdpMulticast_create (sbfUdpMulticastType type,
                                        uint32_t interf,
                                        struct sockaddr_in* address);
void sbfUdpMulticast_destroy (sbfUdpMulticast s);

sbfSocket sbfUdpMulticast_getSocket (sbfUdpMulticast s);

#define sbfUdpMulticast_sendBuffer(s, b) \
    sbfUdpMulticast_send (s, sbfBuffer_getData (b), sbfBuffer_getSize (b))
sbfError sbfUdpMulticast_send (sbfUdpMulticast s, const void* buf, size_t len);

sbfError sbfUdpMulticast_read (sbfUdpMulticast s,
                               sbfUdpMulticastReadCb cb,
                               void* closure,
                               u_int limit);

SBF_END_DECLS

#endif /* _SBF_UDP_MULTICAST_H_ */
