#include "sbfTport.h"
#include "sbfTportPrivate.h"

static void
sbfHandlerBufferDestroyCb (sbfBuffer buffer, void* data, void* closure)
{
    sbfBuffer_destroy (closure);
}

static int
sbfHandlerCheckHeader (sbfTportStream tstream,
                       sbfTportHeader hdr,
                       size_t available)
{
    sbfTport    tport = tstream->mTport;
    size_t      used;
    const char* topic = (const char*)(hdr + 1);

    if (SBF_UNLIKELY (available < sizeof *hdr))
        goto invalid;

    used = (sizeof *hdr) + hdr->mTopicSize + hdr->mSize;
    if (SBF_UNLIKELY (available < used))
        goto invalid;

    if (SBF_UNLIKELY (topic[hdr->mTopicSize - 1] != '\0'))
        goto invalid;
    return 1;

invalid:
    sbfLog_err (tport->mLog, "invalid message on stream %p", tstream);
    return 0;
}

static sbfTportTopic
sbfHandlerFindTopic (sbfTportStream tstream, sbfTportHeader hdr)
{
    sbfTportTopic ttopic;
    const char*   topic = (const char*)(hdr + 1);

    ttopic = sbfTport_findTopic (tstream, topic);
    if (SBF_UNLIKELY (ttopic == NULL))
        return NULL;
    if (SBF_UNLIKELY (TAILQ_EMPTY (&ttopic->mSubs)))
        return NULL;
    return ttopic;
}

static void
sbfHandlerDispatchMessage (sbfTportStream tstream,
                           sbfTportTopic ttopic,
                           sbfBuffer buffer)
{
    sbfSub sub;

    sbfBuffer_lock (buffer);

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
}

sbfHandlerTable*
sbfHandler_load (sbfLog log, const char* type)
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

#ifdef WIN32
    snprintf (path, sizeof path, "sbf%shandler.dll", type);
    handle = LoadLibrary (path);
#else
    snprintf (path, sizeof path, "libsbf%shandler" SBF_SHLIB_SUFFIX, type);
    sbfLog_info (log, "loading %s transport handler (from %s)", type, path);
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
    sbfLog_err (log, "failed to open %s: %s", path, dlerror ());
#endif
    return NULL;
}

size_t
sbfHandler_size (void* data, size_t size)
{
    sbfTportHeader hdr = data;
    size_t         needed;

    if (size < sizeof *hdr)
        return 0;

    needed = (sizeof *hdr) + hdr->mTopicSize + hdr->mSize;
    if (size < needed)
        return 0;
    return needed;
}

void
sbfHandler_packet (sbfHandlerHandle handle, sbfBuffer buffer)
{
    sbfTportStream tstream = handle;
    sbfTportHeader hdr = sbfBuffer_getData (buffer);
    size_t         size = sbfBuffer_getSize (buffer);
    sbfTportTopic  ttopic;
    size_t         used;
    sbfBuffer      newBuffer;

    if (SBF_LIKELY (hdr->mFlags & SBF_MESSAGE_FLAG_LAST_IN_PACKET))
    {
        if (!sbfHandlerCheckHeader (tstream, hdr, size))
            return;

        ttopic = sbfHandlerFindTopic (tstream, hdr);
        if (ttopic == NULL)
            return;
        if (SBF_UNLIKELY (hdr->mFragNum != 0))
        {
            buffer = sbfTport_assemble (tstream, ttopic, hdr);
            if (buffer == NULL)
                return;
        }
        else
        {
            sbfBuffer_setData (buffer, (char*)(hdr + 1) + hdr->mTopicSize);
            sbfBuffer_setSize (buffer, hdr->mSize);
        }
        sbfHandlerDispatchMessage (tstream, ttopic, buffer);
        if (SBF_UNLIKELY (hdr->mFragNum != 0))
            sbfBuffer_destroy (buffer);
        return;
    }

    for (;;)
    {
        if (!sbfHandlerCheckHeader (tstream, hdr, size))
            break;
        used = (sizeof *hdr) + hdr->mTopicSize + hdr->mSize;

        ttopic = sbfHandlerFindTopic (tstream, hdr);
        if (ttopic != NULL)
        {
            if (SBF_UNLIKELY (hdr->mFragNum != 0))
                newBuffer = sbfTport_assemble (tstream, ttopic, hdr);
            else
            {
                sbfBuffer_addRef (buffer);
                newBuffer = sbfBuffer_wrap ((char*)(hdr + 1) + hdr->mTopicSize,
                                            hdr->mSize,
                                            sbfHandlerBufferDestroyCb,
                                            buffer);
            }
            if (newBuffer != NULL)
            {
                sbfHandlerDispatchMessage (tstream, ttopic, newBuffer);
                sbfBuffer_destroy (newBuffer);
            }
        }

        if (hdr->mFlags & SBF_MESSAGE_FLAG_LAST_IN_PACKET)
            break;
        hdr = (sbfTportHeader)((char*)hdr + used);

        size -= used;
   }
}
