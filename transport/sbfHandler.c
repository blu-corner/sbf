#include "sbfTport.h"
#include "sbfTportPrivate.h"

sbfHandlerTable*
sbfHandler_load (const char* type)
{
    char             path[PATH_MAX];
    char             symbol[128];
#ifdef WIN32
    HMODULE          handle;
    LPVOID           s;
#else
    void*            handle;
#endif
    sbfHandlerTable* table;

    sbfLog_info ("loading handler %s", type);

#ifdef WIN32
    snprintf (path, sizeof path, "sbf%shandler.dll", type);
    handle = LoadLibrary (path);
#else
    snprintf (path, sizeof path, "libsbf%shandler.so", type);
    handle = dlopen (path, RTLD_NOW|RTLD_LOCAL);
#endif
    if (handle == NULL)
        goto fail;

    snprintf (symbol, sizeof symbol, "sbf_%s_handler", type);
#ifdef WIN32
    table = (sbfHandlerTable*)GetProcAddress (handle, symbol);
#else
    table = dlsym (handle, symbol);
#endif
    if (table == NULL)
        goto fail;
    return table;

fail:
#ifdef WIN32
    FormatMessage (FORMAT_MESSAGE_ALLOCATE_BUFFER|
                   FORMAT_MESSAGE_FROM_SYSTEM|
                   FORMAT_MESSAGE_IGNORE_INSERTS,
                   NULL,
                   GetLastError (),
                   MAKELANGID (LANG_NEUTRAL, SUBLANG_DEFAULT),
                   (LPTSTR)&s,
                   0,
                   NULL);
    ((char*)s)[strcspn (s, "\r")] = '\0';
    sbfLog_err ("failed to open %s: %s", path, s);
    LocalFree (s);
#else
    sbfLog_err ("failed to open %s: %s", path, dlerror ());
#endif
    return NULL;
}

size_t
sbfHandler_size (void* data, size_t size)
{
    sbfTportHeader* hdr = data;
    size_t          needed;

    if (size < sizeof *hdr)
        return 0;

    needed = (sizeof *hdr) + hdr->mTopicSize + hdr->mSize;
    if (size < needed)
        return 0;
    return needed;
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

    if (TAILQ_EMPTY (&ttopic->mSubs))
        goto no_subscription;

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

no_subscription:
    sbfLog_debug ("message on topic %s but not a subscriber", topic);
    return;

missing:
    sbfLog_debug ("message on unknown topic %s", topic);
    return;

invalid:
    sbfLog_err ("invalid message on stream %p", tstream);
}
