#ifndef _SBF_UDP_HANDLER_H_
#define _SBF_UDP_HANDLER_H_

#include "sbfHandlerInternal.h"
#include "sbfInterface.h"
#include "sbfMw.h"
#include "sbfMwInternal.h"
#include "sbfTport.h"
#include "sbfUdpMulticast.h"

SBF_BEGIN_DECLS

/*
 * UDP handler. These may be specified in the properties:
 *
 * interface - interface name or address
 *
 * range - as a CIDR address (239/8, 239.1/16, etc), at least eight bits must
 *         be available for the host portion
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
    in_addr_t                          mAddress;
    sbfUdpMulticast                    mSocket;

    sbfHandlerAddStreamCompleteCb      mAddStreamCompleteCb;
    void*                              mClosure;

    struct event                       mEventAdd;
    struct event                       mEventListen;

    RB_ENTRY (sbfUdpHandlerStreamImpl) mTreeEntry;
};
typedef struct sbfUdpHandlerStreamImpl* sbfUdpHandlerStream;

RB_HEAD (sbfUdpHandlerStreamTreeImpl, sbfUdpHandlerStreamImpl);
typedef struct sbfUdpHandlerStreamTreeImpl sbfUdpHandlerStreamTree;

static SBF_INLINE int
sbfUdpHandlerStreamCmp (sbfUdpHandlerStream lhs, sbfUdpHandlerStream rhs)
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
}
RB_GENERATE_STATIC (sbfUdpHandlerStreamTreeImpl,
                    sbfUdpHandlerStreamImpl,
                    mTreeEntry,
                    sbfUdpHandlerStreamCmp)

struct sbfUdpHandlerImpl
{
    in_addr_t               mInterface;

    in_addr_t               mBase;

    u_int                   mFirstSize;
    u_int                   mSecondSize;
    u_int                   mThirdSize;

    u_int                   mFirstShift;
    u_int                   mSecondShift;

    sbfPool                 mPool;
    sbfUdpHandlerStreamTree mTree;
};
typedef struct sbfUdpHandlerImpl* sbfUdpHandler;

SBF_END_DECLS

#endif /* _SBF_UDP_HANDLER_H_ */
