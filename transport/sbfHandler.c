#include "sbfTport.h"
#include "sbfTportPrivate.h"

sbfHandlerTable*
sbfHandler_load (const char* type)
{
    char             path[PATH_MAX];
    char             symbol[128];
    void*            handle;
    sbfHandlerTable* table;

    sbfLog_info ("loading handler %s", type);

    snprintf (path, sizeof path, "libsbf%shandler.so", type);
    handle = dlopen (path, RTLD_NOW|RTLD_LOCAL);
    if (handle == NULL)
        goto fail;

    snprintf (symbol, sizeof symbol, "sbf_%s_handler", type);
    table = dlsym (handle, symbol);
    if (table == NULL)
        goto fail;
    return table;

fail:
    sbfLog_err ("failed to open %s: %s", path, dlerror ());
    return NULL;
}

void
sbfHandler_message (sbfHandlerHandle handle, sbfBuffer buffer)
{
    sbfTportStream  tstream = handle;
    sbfTportTopic   ttopic;
    sbfTportHeader* hdr = sbfBuffer_getData (buffer);
    size_t          size = sbfBuffer_getSize (buffer);
    const char*     topic;
    sbfSub          sub;

    if (size < sizeof *hdr)
        goto invalid;
    if (size < (sizeof *hdr) + hdr->mTopicSize + hdr->mSize)
        goto invalid;

    topic = (void*)(hdr + 1);
    if (topic[hdr->mTopicSize - 1] != '\0')
        goto invalid;

    ttopic = sbfTport_findTopic (tstream, topic);
    if (ttopic == NULL)
        goto missing;

    sbfBuffer_setData (buffer, (char*)(hdr + 1) + hdr->mTopicSize);
    sbfBuffer_setSize (buffer, hdr->mSize);

    if (ttopic->mNext == NULL)
        ttopic->mNext = TAILQ_FIRST (&ttopic->mSubs);
    sub = ttopic->mNext;
    do
    {
        sbfSub_message (sub, buffer);

        sub = TAILQ_NEXT (sub, mEntry);
        if (sub == NULL)
            sub = TAILQ_FIRST (&ttopic->mSubs);
    }
    while (sub != ttopic->mNext);
    ttopic->mNext = TAILQ_NEXT (sub, mEntry);

    return;

missing:
    sbfLog_debug ("message on unknown topic %s", topic);
    return;

invalid:
    sbfLog_err ("invalid message on stream %p", tstream);
}
