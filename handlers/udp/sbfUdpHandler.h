#ifndef _SBF_UDP_HANDLER_H_
#define _SBF_UDP_HANDLER_H_

#include "sbfHandler.h"
#include "sbfInterface.h"
#include "sbfMw.h"
#include "sbfTport.h"
#include "sbfUdpMulticast.h"
#include "sbfCommonHandler.h"

SBF_BEGIN_DECLS

/*
 * UDP handler. These may be specified in the properties:
 *
 * udp.interface - interface name or address
 *
 * udp. range - as a CIDR address (239/8, 239.1/16, etc), at least eight 
 *              bits must be available for the host portion
 *
 * Multicast address is automatically allocated based on the topic.
 */

struct sbfUdpHandlerImpl;

#define SBF_UDP_HANDLER_DEFAULT_PORT 33333

#define SBF_UDP_HANDLER_READ_LIMIT 64

struct sbfUdpHandlerStreamImpl
{
    struct sbfUdpHandlerImpl*          mParent;

    sbfMwThread                        mThread;
    sbfHandlerHandle                   mHandle;

    sbfTopicType                       mType;
    uint32_t                           mAddress;
    sbfUdpMulticast                    mSocket;

    struct event                       mEventAdd;
    sbfHandlerAddStreamCompleteCb      mAddStreamCompleteCb;
    void*                              mClosure;

    struct event                       mEventListen;

    RB_ENTRY (sbfUdpHandlerStreamImpl) mTreeEntry;
};
typedef struct sbfUdpHandlerStreamImpl* sbfUdpHandlerStream;

SBF_RB_TREE (sbfUdpHandlerStream, Tree, mTreeEntry,
{
    if (lhs->mType != rhs->mType)
    {
        if (lhs->mType == SBF_TOPIC_PUB)
            return -1;
        return 1;
    }

    if (lhs->mAddress < rhs->mAddress)
        return -1;
    if (lhs->mAddress > rhs->mAddress)
        return 1;
    return 0;
})

struct sbfUdpHandlerImpl
{
    sbfLog                  mLog;

    uint32_t                mInterface;

    sbfMcastTopicResolver   mMtr;

    sbfPool                 mPool;
    sbfUdpHandlerStreamTree mTree;
};
typedef struct sbfUdpHandlerImpl* sbfUdpHandler;

SBF_END_DECLS

#endif /* _SBF_UDP_HANDLER_H_ */
