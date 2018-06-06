#include "sbfTport.h"
#include "sbfTportPrivate.h"

static void
sbfTportFragmentBufferDestroyCb (sbfBuffer buffer, void* data, void* closure)
{
    free (data);
}

void
sbfTport_fragment (sbfTportStream tstream,
                   sbfBuffer buffer,
                   sbfTportHeader hdr,
                   size_t hdrSize)
{
    sbfTport       tport = tstream->mTport;
    void*          data;
    size_t         size;
    size_t         offset;
    uint8_t        fragnum;
    sbfBuffer      fragbuffer;
    size_t         fragsize;
    sbfTportHeader fraghdr;

    SBF_ASSERT (tport->mHandlerTable->mPacketSize >= hdrSize);

    data = sbfBuffer_getData (buffer);
    size = sbfBuffer_getSize (buffer);

    fragnum = 1;

    sbfMutex_lock (&tstream->mSendLock);

    offset = 0;
    while (offset < size)
    {
        fragsize = SBF_MIN (tport->mHandlerTable->mPacketSize - hdrSize,
                            size - offset);
        fragbuffer = sbfTport_getBuffer (tstream, fragsize + hdrSize);

        fraghdr = sbfBuffer_getData (fragbuffer);
        memcpy (fraghdr, hdr, hdrSize);
        fraghdr->mSize = fragsize;
        fraghdr->mFlags = SBF_MESSAGE_FLAG_LAST_IN_PACKET;
        fraghdr->mFragNum = fragnum++;

        memcpy ((char*)fraghdr + hdrSize, (char*)data + offset, fragsize);

        offset += fragsize;
        if (offset == size)
            fraghdr->mFlags |= SBF_MESSAGE_FLAG_LAST_FRAGMENT;

        tport->mHandlerTable->mSendBuffer (tstream->mStream, fragbuffer);
    }

    sbfMutex_unlock (&tstream->mSendLock);
}

sbfBuffer
sbfTport_assemble (sbfTportStream tstream,
                   sbfTportTopic ttopic,
                   sbfTportHeader hdr)
{
    sbfBuffer returned;

    if (hdr->mFragNum != ttopic->mFragNum + 1)
        goto invalid;

    if (ttopic->mFragData == NULL)
    {
        if (hdr->mFragNum != 1)
            goto invalid;

        ttopic->mFragAllocated = 8192;
        ttopic->mFragData = xmalloc (ttopic->mFragAllocated);

        ttopic->mFragNum = 1;
        ttopic->mFragSize = 0;
    }
    else
        ttopic->mFragNum = hdr->mFragNum;

    while (ttopic->mFragAllocated < ttopic->mFragSize + hdr->mSize)
    {
        ttopic->mFragData = xrealloc (ttopic->mFragData,
                                      2,
                                      ttopic->mFragAllocated);
        ttopic->mFragAllocated *= 2;
    }

    memcpy ((char*)ttopic->mFragData + ttopic->mFragSize,
            (char*)(hdr + 1) + hdr->mTopicSize,
            hdr->mSize);
    ttopic->mFragSize += hdr->mSize;

    if (!(hdr->mFlags & SBF_MESSAGE_FLAG_LAST_FRAGMENT))
        return NULL;
    returned = sbfBuffer_wrap (ttopic->mFragData,
                               ttopic->mFragSize,
                               sbfTportFragmentBufferDestroyCb,
                               NULL);

    ttopic->mFragData = NULL;
    ttopic->mFragNum = 0;
    return returned;

invalid:
    if (ttopic->mFragData != NULL)
        free (ttopic->mFragData);

    ttopic->mFragData = NULL;
    ttopic->mFragNum = 0;
    return NULL;
}
