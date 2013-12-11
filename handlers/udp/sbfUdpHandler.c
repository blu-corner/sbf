#include "sbfUdpHandler.h"

static uint32_t
sbfUdpHandlerParseRange (const char* s, u_int* bits)
{
    int          found;
    union
    {
        uint32_t address;
        uint8_t  octets[4];
    } u = { 0 };

    found = 0;
    if (sscanf (s,
                "%hhu.%hhu.%hhu.%hhu/%u",
                &u.octets[0],
                &u.octets[1],
                &u.octets[2],
                &u.octets[3],
                bits) == 5)
        found = 1;
    else if (sscanf (s,
                     "%hhu.%hhu.%hhu/%u",
                     &u.octets[0],
                     &u.octets[1],
                     &u.octets[2],
                     bits) == 4)
        found = 1;
    else if (sscanf (s, "%hhu.%hhu/%u", &u.octets[0], &u.octets[1], bits) == 3)
        found = 1;
    else if (sscanf (s, "%hhu/%u", &u.octets[0], bits) == 2)
        found = 1;
    if (!found)
        return 0;
    return u.address;
}

static uint32_t
sbfUdpHandlerMakeAddress (sbfUdpHandler uh, sbfTopic topic)
{
    uint32_t h;
    uint32_t address;

    address = ntohl (uh->mBase);

    h = hash32_str (sbfTopic_getFirst (topic), HASHINIT);
    address |= (h % uh->mFirstSize) << uh->mFirstShift;

    h = hash32_str (sbfTopic_getSecond (topic), HASHINIT);
    address |= (h % uh->mSecondSize) << uh->mSecondShift;

    h = hash32_str (sbfTopic_getThird (topic), HASHINIT);
    address |= 1 + (h % uh->mThirdSize);

    return htonl (address);
}

static sbfError
sbfUdpHandlerReadBufferCb (sbfUdpMulticast s, sbfBuffer buffer, void* closure)
{
    sbfUdpHandlerStream uhs = closure;

    sbfHandler_message (uhs->mHandle, buffer);

    return 0;
}

static void
sbfUdpHandlerReadEventCb (int fd, short events, void* closure)
{
    sbfUdpHandlerStream uhs = closure;

    sbfUdpMulticast_read (uhs->mSocket,
                          sbfUdpHandlerReadBufferCb,
                          uhs,
                          SBF_UDP_HANDLER_READ_LIMIT);
}

static void
sbfUdpHandlerAddCompleteEventCb (int fd, short events, void* closure)
{
    sbfUdpHandlerStream uhs = closure;
    sbfUdpHandler       uh = uhs->mParent;
    sbfError            error;
    sbfUdpMulticastType type;
    struct sockaddr_in  address;

    if (uhs->mType == SBF_TOPIC_PUB)
        type = SBF_UDP_MULTICAST_SEND;
    else
        type = SBF_UDP_MULTICAST_LISTEN;

    memset (&address, 0, sizeof address);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = uhs->mAddress;
    address.sin_port = htons (SBF_UDP_HANDLER_DEFAULT_PORT);

    uhs->mSocket = sbfUdpMulticast_create (type, uh->mInterface, &address);
    if (uhs->mSocket == NULL)
        error = errno;
    else
    {
        error = 0;

        if (type == SBF_UDP_MULTICAST_LISTEN)
        {
            event_assign (&uhs->mEventListen,
                          sbfMw_getThreadEventBase (uhs->mThread),
                          sbfUdpMulticast_getSocket (uhs->mSocket),
                          EV_READ|EV_PERSIST,
                          sbfUdpHandlerReadEventCb,
                          uhs);
            event_add (&uhs->mEventListen, NULL);
        }
    }

    uhs->mAddStreamCompleteCb (uhs->mHandle, error, uhs->mClosure);
}

static sbfHandler
sbfUdpHandlerCreate (sbfTport tport, sbfKeyValue properties)
{
    sbfUdpHandler uh;
    const char*   s;
    uint32_t      interface;
    uint32_t      range;
    u_int         bits;
    u_int         first;
    u_int         second;
    u_int         third;
    char          tmp[INET_ADDRSTRLEN];

    s = sbfKeyValue_get (properties, "interface");
    if (s == NULL)
        s = "eth0";
    interface = sbfInterface_find (s);
    if (interface == 0)
    {
        sbfLog_err ("couldn't get interface: %s", s);
        return NULL;
    }

    s = sbfKeyValue_get (properties, "range");
    if (s == NULL)
        s = "239.100/16";
    range = sbfUdpHandlerParseRange (s, &bits);
    if (range == 0)
    {
        sbfLog_err ("couldn't get range: %s", s);
        return NULL;
    }
    if (bits > 24)
    {
        sbfLog_err ("need at least eight bits: %s", s);
        return NULL;
    }
    bits = 32 - bits;

    first = (bits / 6) * 1;
    second = (bits / 6) * 1;
    third = bits - first - second;

    uh = xcalloc (1, sizeof *uh);
    RB_INIT (&uh->mTree);
    uh->mPool = sbfPool_create (65536);

    uh->mInterface = interface;
    uh->mBase = range;

    uh->mFirstSize = 1 + ~(0xffffffffU << first);
    uh->mSecondSize = 1 + ~(0xffffffffU << second);
    uh->mThirdSize = 1 + ~(0xffffffffU << third) - 2;

    uh->mFirstShift = second + third;
    uh->mSecondShift = third;

    inet_ntop (AF_INET, &uh->mBase, tmp, sizeof tmp);
    sbfLog_info ("range %s/%u: first %u<<%u, second %u<<%u, third %u",
                 tmp,
                 bits,
                 uh->mFirstSize,
                 uh->mFirstShift,
                 uh->mSecondSize,
                 uh->mSecondShift,
                 uh->mThirdSize);

    return uh;
}

static void
sbfUdpHandlerDestroy (sbfHandler handler)
{
    sbfUdpHandler uh = handler;

    sbfPool_destroy (uh->mPool);
    free (uh);
}

sbfHandlerHandle
sbfUdpHandlerFindStream (sbfHandler handler, sbfTopic topic)
{
    sbfUdpHandler                  uh = handler;
    struct sbfUdpHandlerStreamImpl impl;
    sbfUdpHandlerStream            uhs;
    char                           tmp[INET_ADDRSTRLEN];

    impl.mType = sbfTopic_getType (topic);
    impl.mAddress = sbfUdpHandlerMakeAddress (uh, topic);

    uhs = RB_FIND (sbfUdpHandlerStreamTreeImpl, &uh->mTree, &impl);
    if (uhs == NULL)
        return NULL;

    inet_ntop (AF_INET, &uhs->mAddress, tmp, sizeof tmp);
    sbfLog_debug ("found stream %p, address %s (for topic %s)",
                  uhs,
                  tmp,
                  sbfTopic_getTopic (topic));

    return uhs->mHandle;
}

sbfHandlerStream
sbfUdpHandlerAddStream (sbfHandler handler,
                        sbfTopic topic,
                        sbfMwThread thread,
                        sbfHandlerHandle handle,
                        sbfHandlerAddStreamCompleteCb cb,
                        void* closure)
{
    sbfUdpHandler       uh = handler;
    sbfUdpHandlerStream uhs;
    char                tmp[INET_ADDRSTRLEN];

    uhs = xcalloc (1, sizeof *uhs);
    uhs->mParent = uh;

    uhs->mThread = thread;
    uhs->mHandle = handle;

    uhs->mType = sbfTopic_getType (topic);
    uhs->mAddress = sbfUdpHandlerMakeAddress (uh, topic);

    inet_ntop (AF_INET, &uhs->mAddress, tmp, sizeof tmp);
    sbfLog_debug ("adding stream %p, address %s (for topic %s)",
                  uhs,
                  tmp,
                  sbfTopic_getTopic (topic));

    RB_INSERT (sbfUdpHandlerStreamTreeImpl, &uh->mTree, uhs);

    uhs->mAddStreamCompleteCb = cb;
    uhs->mClosure = closure;

    if (cb != NULL)
    {
        sbfMw_enqueue (uhs->mThread,
                       &uhs->mEventAdd,
                       sbfUdpHandlerAddCompleteEventCb,
                       uhs);
    }
    return uhs;
}

void
sbfUdpHandlerRemoveStream (sbfHandlerStream stream)
{
    sbfUdpHandlerStream uhs = stream;
    sbfUdpHandler       uh = uhs->mParent;
    char                tmp[INET_ADDRSTRLEN];

    inet_ntop (AF_INET, &uhs->mAddress, tmp, sizeof tmp);
    sbfLog_debug ("removing stream %p, address %s", uhs, tmp);

    if (uhs->mType == SBF_TOPIC_SUB)
        event_del (&uhs->mEventListen);
    event_del (&uhs->mEventAdd);

    sbfUdpMulticast_destroy (uhs->mSocket);

    RB_REMOVE (sbfUdpHandlerStreamTreeImpl, &uh->mTree, uhs);

    free (uhs);
}

static sbfBuffer
sbfUdpHandlerGetBuffer (sbfHandlerStream stream, size_t size)
{
    sbfUdpHandlerStream uhs = stream;
    sbfUdpHandler       uh = uhs->mParent;

    return sbfBuffer_new (uh->mPool, size);
}

static void
sbfUdpHandlerSendBuffer (sbfHandlerStream stream, sbfBuffer buffer)
{
    sbfUdpHandlerStream uhs = stream;

    sbfUdpMulticast_sendBuffer (uhs->mSocket, buffer);
}

sbfHandlerTable sbf_udp_handler =
{
    sbfUdpHandlerCreate,
    sbfUdpHandlerDestroy,
    sbfUdpHandlerFindStream,
    sbfUdpHandlerAddStream,
    sbfUdpHandlerRemoveStream,
    sbfUdpHandlerGetBuffer,
    sbfUdpHandlerSendBuffer
};
