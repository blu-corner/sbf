#include "sbfMw.h"
#include "sbfMwPrivate.h"
#include "sbfPerfCounter.h"

static void
sbfMwEventBaseTimerCb (int fd, short events, void* closure)
{
    sbfMwThread    thread = closure;
    struct timeval tv;

    timerclear (&tv);
    tv.tv_sec = 3600;

    event_add (&thread->mTimer, &tv);
}

static void*
sbfMwEventBaseThreadCb (void* closure)
{
    sbfMwThread thread = closure;

    sbfLog_debug (thread->mParent->mLog, "thread %u entered", thread->mIndex);

    while (!thread->mParent->mStop)
        event_base_loop (thread->mEventBase, 0);

    sbfLog_debug (thread->mParent->mLog, "thread %u exited", thread->mIndex);

    return NULL;
}

sbfMw
sbfMw_create (sbfLog log, sbfKeyValue properties)
{
    sbfMw                mw;
    const char*          value;
    int                  threads;
    u_int                i;
    sbfMwThread          thread;
    struct event_config* ec;
#ifdef WIN32
    WSADATA              wsd;
#endif

    // Check all the supported capabilities
    sbfMw_checkSupported(CAP_ALL_MASK);
    
    value = sbfKeyValue_get (properties, "threads");
    if (value == NULL)
        threads = 1;
    else
        threads = atoi (value);
    if (threads <= 0 || threads > SBF_MW_THREAD_LIMIT)
        return NULL;

#ifndef WIN32
    signal (SIGPIPE, SIG_IGN);
#endif

    mw = xcalloc (1, sizeof *mw);
    mw->mProperties = sbfKeyValue_copy (properties);
    mw->mLog = log;
    mw->mNumThreads = threads;
    mw->mThreads = xcalloc (mw->mNumThreads, sizeof *mw->mThreads);

    sbfLog_info (mw->mLog, "this is version " SBF_VERSION);
    sbfLog_debug (mw->mLog,
                  "creating middleware %p, using %u threads",
                  mw,
                  threads);

#ifdef WIN32
    if (WSAStartup(MAKEWORD (2, 2), &wsd) != 0)
        SBF_FATAL ("WSAStartup failed");
    if (evthread_use_windows_threads () != 0)
        SBF_FATAL ("event_use_windows_threads failed");
#else
    if (evthread_use_pthreads () != 0)
        SBF_FATAL ("event_use_pthreads failed");
#endif

    ec = event_config_new ();
    event_config_set_flag (ec, EVENT_BASE_FLAG_EPOLL_USE_CHANGELIST);

    for (i = 0; i < mw->mNumThreads; i++)
    {
        thread = &mw->mThreads[i];
        thread->mParent = mw;
        thread->mIndex = i;

        thread->mEventBase = event_base_new_with_config (ec);
        if (thread->mEventBase == NULL)
            SBF_FATAL ("event_base_new failed");

        event_assign (&thread->mTimer,
                      thread->mEventBase,
                      -1,
                      0,
                      sbfMwEventBaseTimerCb,
                      thread);
        sbfMwEventBaseTimerCb (-1, 0, thread);

        if (sbfThread_create (&thread->mThread,
                              sbfMwEventBaseThreadCb,
                              thread) != 0)
            SBF_FATAL ("sbfThread_create failed");
    }

    event_config_free (ec);
    return mw;
}

void
sbfMw_destroy (sbfMw mw)
{
    u_int i;

    sbfLog_debug (mw->mLog, "destroying middleware %p", mw);

    mw->mStop = 1;
    for (i = 0; i < mw->mNumThreads; i++)
    {
        event_del (&mw->mThreads[i].mTimer);
        event_base_loopexit (mw->mThreads[i].mEventBase, NULL);
    }
    for (i = 0; i < mw->mNumThreads; i++)
    {
        sbfThread_join (mw->mThreads[i].mThread);
        event_base_free (mw->mThreads[i].mEventBase);
    }
    free (mw->mThreads);

    sbfKeyValue_destroy (mw->mProperties);
    free (mw);
}

sbfMwThread
sbfMw_getThread (sbfMw mw, u_int index)
{
    if (index >= mw->mNumThreads)
        return NULL;
    return &mw->mThreads[index];
}

u_int
sbfMw_getNumThreads (sbfMw mw)
{
    return mw->mNumThreads;
}

u_int
sbfMw_getThreadIndex (sbfMwThread thread)
{
    return thread->mIndex;
}

uint64_t
sbfMw_getThreadMask (sbfMwThread thread)
{
    return 1ULL << thread->mIndex;
}

struct event_base*
sbfMw_getThreadEventBase (sbfMwThread thread)
{
    return thread->mEventBase;
}

void
sbfMw_enqueueThread (sbfMwThread thread,
                     struct event* event,
                     event_callback_fn cb,
                     void* closure)
{
    event_assign (event,
                  sbfMw_getThreadEventBase (thread),
                  -1,
                  EV_TIMEOUT,
                  cb,
                  closure);
    event_active (event, EV_TIMEOUT, 1);
}

sbfLog
sbfMw_getLog (sbfMw mw)
{
    return mw->mLog;
}

sbfKeyValue
sbfMw_getProperties (sbfMw mw)
{
    return mw->mProperties;
}

uint32_t
sbfMw_checkSupported(uint32_t cap_mask)
{
    static uint64_t supported = 0LL;
    static char processed = 0;

    // Performs the check for the first time and cache it into static variable
    if (processed == 0)
    {
        // Hi resolution counter capability
        supported = (sbfPerfCounter_frequency() > 0)? CAP_HI_RES_COUNTER: supported;

        // TODO: Extends other capabilities here...

        processed = 1;
    }
    
    return cap_mask & (uint32_t) supported;
}


