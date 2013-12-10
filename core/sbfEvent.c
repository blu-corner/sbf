#include "sbfEvent.h"
#include "sbfEventPrivate.h"

#if SBF_EVENT_READ != EV_READ || SBF_EVENT_WRITE != EV_WRITE
#error "event numbers do not match"
#endif

static void
sbfEventQueueCb (sbfQueueItem item, void* closure)
{
    sbfEvent event = closure;
    int      events;

    if (!event->mDestroyed)
    {
        events = *(int*)sbfQueue_getItemData (item);
        event->mCb (event, events, event->mClosure);
    }

    if (sbfRefCount_decrement (&event->mRefCount))
        free (event);
}

static void
sbfEventEventCb (int fd, short events, void* closure)
{
    sbfEvent     event = closure;
    sbfQueueItem item;

    event_add (&event->mEvent, NULL);

    if (!event->mDestroyed)
    {
        sbfRefCount_increment (&event->mRefCount);
        item = sbfQueue_getItem (event->mQueue, sbfEventQueueCb, event);
        *(int*)sbfQueue_getItemData (item) = events;
        sbfQueue_enqueueItem (event->mQueue, item);
    }
}

sbfEvent
sbfEvent_create (sbfMwThread thread,
                 sbfQueue queue,
                 sbfEventCb cb,
                 void* closure,
                 int descriptor,
                 int events)
{
    sbfEvent event;

    event = xmalloc (sizeof *event);
    event->mThread = thread;
    event->mQueue = queue;

    event->mCb = cb;
    event->mClosure = closure;

    event->mDestroyed = 0;
    sbfRefCount_init (&event->mRefCount, 1);

    sbfLog_debug ("creating %p: descriptor %d, events %s%s",
                  event,
                  descriptor,
                  events & SBF_EVENT_READ ? "R" : "",
                  events & SBF_EVENT_WRITE ? "W" : "");

    event_assign (&event->mEvent,
                  event->mThread->mEventBase,
                  descriptor,
                  events,
                  sbfEventEventCb,
                  event);
    event_add (&event->mEvent, NULL);

    return event;
}

void
sbfEvent_destroy (sbfEvent event)
{
    sbfLog_debug ("destroying %p", event);

    event->mDestroyed = 1;
    event_del (&event->mEvent);

    if (sbfRefCount_decrement (&event->mRefCount))
        free (event);
}
