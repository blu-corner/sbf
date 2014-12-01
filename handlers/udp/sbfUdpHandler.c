#include "sbfUdpHandler.h"

static sbfError
sbfUdpHandlerReadBufferCb (sbfUdpMulticast s, sbfBuffer buffer, void* closure)
{
    sbfUdpHandlerStream uhs = closure;

    sbfHandler_packet (uhs->mHandle, buffer);

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
    sbfLog                log = sbfMw_getLog (sbfTport_getMw (tport));
    sbfUdpHandler         uh;
    sbfMcastTopicResolver mtr;
    const char*           s;
    uint32_t              interf;

    s = sbfKeyValue_get (properties, "udp.interface");
    if (s == NULL)
        s = "eth0";
    interf = sbfInterface_findOne (log, s, &s);
    if (interf == 0)
    {
        sbfLog_err (log, "couldn't get interface: %s", s);
        return NULL;
    }

    mtr = sbfMcastTopicResolver_create (sbfKeyValue_get (properties,
                                                         "udp.range"),
                                        log);
    if (mtr == NULL)
    {
        sbfLog_err (log, "couldn't get multicast topic resolver");
        return NULL;
    }

    uh = xcalloc (1, sizeof *uh);
    uh->mLog = log;
    RB_INIT (&uh->mTree);
    uh->mPool = sbfBuffer_createPool (65536);

    uh->mInterface = interf;
    uh->mMtr = mtr;

    return uh;
}

static void
sbfUdpHandlerDestroy (sbfHandler handler)
{
    sbfUdpHandler uh = handler;
    sbfMcastTopicResolver_destroy (uh->mMtr);
    sbfPool_destroy (uh->mPool);
    free (uh);
}

static sbfHandlerHandle
sbfUdpHandlerFindStream (sbfHandler handler, sbfTopic topic)
{
    sbfUdpHandler                  uh = handler;
    struct sbfUdpHandlerStreamImpl impl;
    sbfUdpHandlerStream            uhs;
    char                           tmp[INET_ADDRSTRLEN];

    impl.mType = sbfTopic_getType (topic);
    impl.mAddress = sbfMcastTopicResolver_makeAddress (uh->mMtr, topic);

    uhs = RB_FIND (sbfUdpHandlerStreamTreeImpl, &uh->mTree, &impl);
    if (uhs == NULL)
        return NULL;

    inet_ntop (AF_INET, &uhs->mAddress, tmp, sizeof tmp);
    sbfLog_debug (uh->mLog,
                  "found stream %p, address %s (for topic %s)",
                  uhs,
                  tmp,
                  sbfTopic_getTopic (topic));

    return uhs->mHandle;
}

static sbfHandlerStream
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
    uhs->mAddress = sbfMcastTopicResolver_makeAddress (uh->mMtr, topic);

    inet_ntop (AF_INET, &uhs->mAddress, tmp, sizeof tmp);
    sbfLog_debug (uh->mLog,
                  "adding stream %p, address %s (for topic %s)",
                  uhs,
                  tmp,
                  sbfTopic_getTopic (topic));

    RB_INSERT (sbfUdpHandlerStreamTreeImpl, &uh->mTree, uhs);

    uhs->mAddStreamCompleteCb = cb;
    uhs->mClosure = closure;

    if (cb != NULL)
    {
        sbfMw_enqueueThread (uhs->mThread,
                             &uhs->mEventAdd,
                             sbfUdpHandlerAddCompleteEventCb,
                             uhs);
    }

    return uhs;
}

static void
sbfUdpHandlerRemoveStream (sbfHandlerStream stream)
{
    sbfUdpHandlerStream uhs = stream;
    sbfUdpHandler       uh = uhs->mParent;
    char                tmp[INET_ADDRSTRLEN];

    inet_ntop (AF_INET, &uhs->mAddress, tmp, sizeof tmp);
    sbfLog_debug (uh->mLog, "removing stream %p, address %s", uhs, tmp);

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
    65507,
    sbfUdpHandlerCreate,
    sbfUdpHandlerDestroy,
    sbfUdpHandlerFindStream,
    sbfUdpHandlerAddStream,
    sbfUdpHandlerRemoveStream,
    sbfUdpHandlerGetBuffer,
    sbfUdpHandlerSendBuffer
};
