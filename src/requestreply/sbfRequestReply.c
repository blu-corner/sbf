#include "sbfRequestReply.h"
#include "sbfRequestReplyPrivate.h"
#include "sbfRequestPubPrivate.h"
#include "sbfRequestSubPrivate.h"

void
sbfRequest_reply (sbfRequest req, void* data, size_t size)
{
    if (req->mSub == NULL)
        SBF_FATAL ("can't reply to a sent request");
    sbfRequestSub_reply (req->mSub, req, data, size);
}

void
sbfRequest_detach (sbfRequest req)
{
    sbfRefCount_increment (&req->mRefCount);
}

void
sbfRequest_destroy (sbfRequest req)
{
    if (!sbfRefCount_decrement (&req->mRefCount))
        return;

    if (req->mPub != NULL)
        RB_REMOVE (sbfRequestTreeImpl, &req->mPub->mRequests, req);

    free (req);
}
