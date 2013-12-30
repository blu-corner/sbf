#include "sbfMw.h"
#include "sbfMwPrivate.h"

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

    sbfLog_debug ("thread %u entered", thread->mIndex);

    while (!thread->mParent->mStop)
        event_base_loop (thread->mEventBase, 0);

    sbfLog_debug ("thread %u exited", thread->mIndex);

    return NULL;
}

sbfMw
sbfMw_create (u_int threads)
{
    sbfMw                mw;
    u_int                i;
    sbfMwThread          thread;
    struct event_config* ec;
#ifdef WIN32
    WSADATA              wsd;
#endif

    if (threads == 0 || threads > SBF_MW_THREAD_LIMIT)
        return NULL;

#ifdef linux
    signal (SIGPIPE, SIG_IGN);
#endif

    mw = xcalloc (1, sizeof *mw);
    mw->mNumThreads = threads;
    mw->mThreads = xcalloc (mw->mNumThreads, sizeof *mw->mThreads);

    sbfLog_info ("this is version " SBF_VERSION);
    sbfLog_debug ("creating %p, using %u threads", mw, threads);

#ifdef WIN32
    if (WSAStartup(MAKEWORD (2, 2), &wsd) != 0)
        SBF_FATAL ("WSAStartup failed");
    if (evthread_use_windows_threads() != 0)
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

    return mw;
}

void
sbfMw_destroy (sbfMw mw)
{
    u_int i;

    sbfLog_debug ("destroying %p", mw);

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
