#include "sbfRequestReply.h"
#include "sbfRequestReplyPrivate.h"
#include "sbfRequestPubPrivate.h"

static void
sbfRequestPubPubReadyCb (sbfPub pub0, void* closure)
{
    sbfRequestPub pub = closure;

    pub->mPubReady = 1;
    if (pub->mSubReady && pub->mReadyCb != NULL)
        pub->mReadyCb (pub, pub->mClosure);
}

static void
sbfRequestPubSubReadyCb (sbfSub sub, void* closure)
{
    sbfRequestPub pub = closure;

    pub->mSubReady = 1;
    if (pub->mPubReady && pub->mReadyCb != NULL)
        pub->mReadyCb (pub, pub->mClosure);
}

static void
sbfRequestPubSubMessageCb (sbfSub sub, sbfBuffer buffer, void* closure)
{
    sbfRequestPub         pub = closure;
    sbfRequestHeader*     hdr = sbfBuffer_getData (buffer);
    size_t                size = sbfBuffer_getSize (buffer);
    sbfBuffer             new;
    struct sbfRequestImpl wanted;
    sbfRequest            req;

    if (size < sizeof *hdr)
        return;

    sbfGuid_copy (&wanted.mGuid, &hdr->mGuid);

    req = RB_FIND (sbfRequestTreeImpl, &pub->mRequests, &wanted);
    if (req != NULL && req->mReplyCb != NULL)
    {
        new = sbfBuffer_wrap (hdr + 1, size - sizeof *hdr, NULL, NULL);
        req->mReplyCb (pub, req, new, req->mClosure);
        sbfBuffer_destroy (new);
    }
}

sbfRequestPub
sbfRequestPub_create (sbfTport tport,
                      sbfQueue queue,
                      const char* topic,
                      sbfRequestPubReadyCb readyCb,
                      void* closure)
{
    sbfRequestPub pub;
    char          requestTopic[SBF_TOPIC_SIZE_LIMIT];
    char          replyTopic[SBF_TOPIC_SIZE_LIMIT];
    int           used;

    used = snprintf (requestTopic, sizeof requestTopic, "%s@request", topic);
    if (used < 0 || (size_t)used >= sizeof requestTopic)
        return NULL;
    used = snprintf (replyTopic, sizeof replyTopic, "%s@reply", topic);
    if (used < 0 || (size_t)used >= sizeof replyTopic)
        return NULL;

    pub = xcalloc (1, sizeof *pub);
    pub->mLog = sbfMw_getLog (sbfTport_getMw (tport));
    pub->mTopic = xstrdup (topic);

    sbfLog_debug (pub->mLog,
                  "creating request publisher %p: topic %s",
                  pub,
                  topic);

    pub->mReadyCb = readyCb;
    pub->mClosure = closure;

    sbfGuid_new (pub->mLog, &pub->mNext);
    RB_INIT (&pub->mRequests);

    pub->mPub = sbfPub_create (tport,
                               queue,
                               requestTopic,
                               sbfRequestPubPubReadyCb,
                               pub);
    if (pub->mPub == NULL)
        goto fail;

    pub->mSub = sbfSub_create (tport,
                               queue,
                               replyTopic,
                               sbfRequestPubSubReadyCb,
                               sbfRequestPubSubMessageCb,
                               pub);
    if (pub->mSub == NULL)
        goto fail;

    return pub;

fail:
    sbfRequestPub_destroy (pub);
    return NULL;
}

void
sbfRequestPub_destroy (sbfRequestPub pub)
{
    sbfRequest req;
    sbfRequest req1;

    sbfLog_debug (pub->mLog, "destroying request publisher %p", pub);

    if (pub->mPub != NULL)
        sbfPub_destroy (pub->mPub);
    if (pub->mSub != NULL)
        sbfSub_destroy (pub->mSub);

    RB_FOREACH_SAFE (req, sbfRequestTreeImpl, &pub->mRequests, req1)
        sbfRequest_destroy (req);

    free ((void*)pub->mTopic);
    free (pub);
}

sbfRequest
sbfRequestPub_send (sbfRequestPub pub,
                    void* data,
                    size_t size,
                    sbfRequestPubReplyCb replyCb,
                    void* closure)
{
    sbfRequest        req;
    sbfBuffer         buffer;
    sbfRequestHeader* hdr;

    if (!pub->mPubReady || !pub->mSubReady)
        return NULL;

    req = xcalloc (1, sizeof *req);
    req->mPub = pub;
    sbfRefCount_init (&req->mRefCount, 1);

    req->mReplyCb = replyCb;
    req->mClosure = closure;

    sbfGuid_increment (&pub->mNext, 1);
    sbfGuid_copy (&req->mGuid, &pub->mNext);

    RB_INSERT (sbfRequestTreeImpl, &pub->mRequests, req);

    buffer = sbfPub_getBuffer (pub->mPub, size + sizeof *hdr);

    hdr = sbfBuffer_getData (buffer);
    sbfGuid_copy (&hdr->mGuid, &req->mGuid);
    memset (hdr->mSpace, 0, sizeof hdr->mSpace);

    memcpy (hdr + 1, data, size);
    sbfPub_sendBuffer (pub->mPub, buffer);

    return req;
}

const char*
sbfRequestPub_getTopic (sbfRequestPub pub)
{
    return pub->mTopic;
}
