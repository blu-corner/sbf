#include "sbfHiResTimer.h"
#include "sbfHiResTimerPrivate.h"
#include "sbfQueuePrivate.h"

static void
sbfHiResTimerQueueCb (sbfQueueItem item, void* closure)
{
    sbfHiResTimer timer = closure;

    if (!timer->mDestroyed)
        timer->mCb (timer, timer->mClosure);

    if (sbfRefCount_decrement (&timer->mRefCount))
        free (timer);
}

void
sbfHiResTimer_queueDispatch (sbfQueue queue)
{
    sbfHiResTimer timer;
    uint64_t      now;

    if (SBF_UNLIKELY (RB_EMPTY (&queue->mHiResTimers)))
        return;

    for (;;)
    {
        now = sbfPerfCounter_get ();

        timer = RB_MIN (sbfHiResTimerTreeImpl, &queue->mHiResTimers);
        if (timer == NULL || now < timer->mNext)
            break;

        RB_REMOVE (sbfHiResTimerTreeImpl, &queue->mHiResTimers, timer);
        if (!timer->mDestroyed)
        {
            sbfRefCount_increment (&timer->mRefCount);
            sbfQueue_enqueue (timer->mQueue, sbfHiResTimerQueueCb, timer);

            timer->mNext = now + timer->mTicks;
            RB_INSERT (sbfHiResTimerTreeImpl, &queue->mHiResTimers, timer);
        }
        else
        {
            if (sbfRefCount_decrement (&timer->mRefCount))
                free (timer);
        }
    }
}

void
sbfHiResTimer_queueDestroy (sbfQueue queue)
{
    sbfHiResTimer timer;
    sbfHiResTimer timer1;

    RB_FOREACH_SAFE (timer, sbfHiResTimerTreeImpl, &queue->mHiResTimers, timer1)
    {
        sbfLog_debug (timer->mQueue->mLog, "destroying hi-res timer %p", timer);

        RB_REMOVE (sbfHiResTimerTreeImpl, &queue->mHiResTimers, timer);
        timer->mDestroyed = 1;

        if (sbfRefCount_decrement (&timer->mRefCount))
            free (timer);
    }
}

sbfHiResTimer
sbfHiResTimer_create (sbfQueue queue,
                      sbfHiResTimerCb cb,
                      void* closure,
                      u_int interval)
{
    sbfHiResTimer timer;

    timer = xmalloc (sizeof *timer);
    timer->mQueue = queue;

    timer->mInterval = interval;

    timer->mTicks = sbfPerfCounter_ticks (timer->mInterval);
    timer->mNext = sbfPerfCounter_get () + timer->mTicks;

    timer->mCb = cb;
    timer->mClosure = closure;

    timer->mDestroyed = 0;
    sbfRefCount_init (&timer->mRefCount, 1);

    sbfLog_debug (timer->mQueue->mLog,
                  "creating hi-res timer %p: interval %u microseconds",
                  timer,
                  timer->mInterval);

    RB_INSERT (sbfHiResTimerTreeImpl, &queue->mHiResTimers, timer);

    return timer;
}

void
sbfHiResTimer_destroy (sbfHiResTimer timer)
{
    SBF_ASSERT (!timer->mDestroyed);

    sbfLog_debug (timer->mQueue->mLog, "destroying hi-res timer %p", timer);

    timer->mDestroyed = 1;
}

u_int
sbfHiResTimer_getInterval (sbfHiResTimer timer)
{
    return timer->mInterval;
}
