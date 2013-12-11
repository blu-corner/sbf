#ifndef _SBF_UDP_MULTICAST_PRIVATE_H_
#define _SBF_UDP_MULTICAST_PRIVATE_H_

SBF_BEGIN_DECLS

struct sbfUdpMulticastImpl
{
    sbfUdpMulticastType mType;

    sbfSocket           mSocket;
    struct sockaddr_in  mAddress;

    sbfPool             mPool;
};

SBF_END_DECLS

#endif /* _SBF_UDP_MULTICAST_PRIVATE_H_ */
