#include "sbfTimer.h"
#include "sbfTimerPrivate.h"

static void
sbfTimerQueueCb (sbfQueueItem item, void* closure)
{
    sbfTimer timer = closure;

    if (!timer->mDestroyed)
        timer->mCb (timer, timer->mClosure);

    if (sbfRefCount_decrement (&timer->mRefCount))
        free (timer);
}

static void
sbfTimerEventCb (int fd, short events, void* closure)
{
    sbfTimer timer = closure;

    event_add (&timer->mEvent, &timer->mTime);

    if (!timer->mDestroyed)
    {
        sbfRefCount_increment (&timer->mRefCount);
        sbfQueue_enqueue (timer->mQueue, sbfTimerQueueCb, timer);
    }
}

void
sbfTimer_enable (sbfTimer timer)
{
    event_add (&timer->mEvent, &timer->mTime);
}

sbfTimer
sbfTimer_create_disabled (sbfMwThread thread,
                          sbfQueue queue,
                          sbfTimerCb cb,
                          void* closure,
                          double interval)
{
    sbfTimer timer;

    timer = xmalloc (sizeof *timer);
    timer->mThread = thread;
    timer->mQueue = queue;

    timer->mInterval = interval;

    timer->mCb = cb;
    timer->mClosure = closure;

    timer->mDestroyed = 0;
    sbfRefCount_init (&timer->mRefCount, 1);

    timerclear (&timer->mTime);
    timer->mTime.tv_sec = (u_int)interval;
    timer->mTime.tv_usec = (u_int)((interval - timer->mTime.tv_sec) * 1000000);

    sbfLog_debug (timer->mThread->mParent->mLog,
                  "creating timer %p: interval %.3f seconds",
                  timer,
                  timer->mInterval);

    event_assign (&timer->mEvent,
                  timer->mThread->mEventBase,
                  -1,
                  0,
                  sbfTimerEventCb,
                  timer);

    return timer;
}

sbfTimer
sbfTimer_create (sbfMwThread thread,
                 sbfQueue queue,
                 sbfTimerCb cb,
                 void* closure,
                 double interval)
{
    sbfTimer timer;

    timer = sbfTimer_create_disabled (thread, queue, cb, closure, interval);

    sbfTimer_enable (timer);

    return timer;
}

void
sbfTimer_destroy (sbfTimer timer)
{
    sbfLog_debug (timer->mThread->mParent->mLog, "destroying timer %p", timer);

    timer->mDestroyed = 1;
    event_del (&timer->mEvent);

    if (sbfRefCount_decrement (&timer->mRefCount))
        free (timer);
}

double
sbfTimer_getInterval (sbfTimer timer)
{
    return timer->mInterval;
}
