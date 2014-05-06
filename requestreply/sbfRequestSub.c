#include "sbfRequestReply.h"
#include "sbfRequestReplyPrivate.h"
#include "sbfRequestSubPrivate.h"

static void
sbfRequestSubPubReadyCb (sbfPub pub, void* closure)
{
    sbfRequestSub sub = closure;

    sub->mPubReady = 1;
    if (sub->mSubReady && sub->mReadyCb != NULL)
        sub->mReadyCb (sub, sub->mClosure);
}

static void
sbfRequestSubSubReadyCb (sbfSub sub0, void* closure)
{
    sbfRequestSub sub = closure;

    sub->mSubReady = 1;
    if (sub->mPubReady && sub->mReadyCb != NULL)
        sub->mReadyCb (sub, sub->mClosure);
}

static void
sbfRequestSubSubMessageCb (sbfSub sub0, sbfBuffer buffer, void* closure)
{
    sbfRequestSub     sub = closure;
    sbfRequestHeader* hdr;
    sbfRequest        req;

    if (sbfBuffer_getSize (buffer) < sizeof *hdr)
        return;

    req = xcalloc (1, sizeof *req);
    req->mSub = sub;
    sbfRefCount_init (&req->mRefCount, 1);

    hdr = sbfBuffer_getData (buffer);
    sbfGuid_copy (&req->mGuid, &hdr->mGuid);

    if (sub->mRequestCb != NULL)
    {
        sbfBuffer_setData (buffer, hdr + 1);
        sbfBuffer_setSize (buffer, sbfBuffer_getSize (buffer) - sizeof *hdr);
        sub->mRequestCb (sub, req, buffer, sub->mClosure);
    }

    sbfRequest_destroy (req);
}

sbfRequestSub
sbfRequestSub_create (sbfTport tport,
                      sbfQueue queue,
                      const char* topic,
                      sbfRequestSubReadyCb readyCb,
                      sbfRequestSubRequestCb requestCb,
                      void* closure)
{
    sbfRequestSub sub;
    char          replyTopic[SBF_TOPIC_SIZE_LIMIT];
    int           used;

    used = snprintf (replyTopic, sizeof replyTopic, "%s@", topic);
    if (used < 0 || (size_t)used >= sizeof replyTopic)
        return NULL;

    sub = xcalloc (1, sizeof *sub);

    sbfLog_debug ("creating %p: topic %s", sub, topic);

    sub->mReadyCb = readyCb;
    sub->mRequestCb = requestCb;
    sub->mClosure = closure;

    sub->mPub = sbfPub_create (tport,
                               queue,
                               replyTopic,
                               sbfRequestSubPubReadyCb,
                               sub);
    if (sub->mPub == NULL)
        goto fail;

    sub->mSub = sbfSub_create (tport,
                               queue,
                               topic,
                               sbfRequestSubSubReadyCb,
                               sbfRequestSubSubMessageCb,
                               sub);
    if (sub->mSub == NULL)
        goto fail;

    return sub;

fail:
    sbfRequestSub_destroy (sub);
    return NULL;
}

void
sbfRequestSub_destroy (sbfRequestSub sub)
{
    sbfLog_debug ("destroying %p", sub);

    if (sub->mPub != NULL)
        sbfPub_destroy (sub->mPub);
    if (sub->mSub != NULL)
        sbfSub_destroy (sub->mSub);

    free (sub);
}

sbfTopic
sbfRequestSub_getTopic (sbfRequestSub sub)
{
    return sbfSub_getTopic (sub->mSub);
}

void
sbfRequestSub_reply (sbfRequestSub sub, sbfRequest req, void* data, size_t size)
{
    sbfBuffer         buffer;
    sbfRequestHeader* hdr;

    if (req->mSub != sub)
        SBF_FATAL ("request reply on wrong subscription");

    buffer = sbfPub_getBuffer (sub->mPub, size + sizeof *hdr);

    hdr = sbfBuffer_getData (buffer);
    sbfGuid_copy (&hdr->mGuid, &req->mGuid);
    memset (hdr->mSpace, 0, sizeof hdr->mSpace);

    memcpy (hdr + 1, data, size);
    sbfPub_sendBuffer (sub->mPub, buffer);
}
