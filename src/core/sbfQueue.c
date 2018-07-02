#include "sbfQueue.h"
#include "sbfQueuePrivate.h"

sbfQueue
sbfQueue_create (sbfMw mw, const char* name)
{
    sbfQueue    queue;
    const char* value;

    queue = xcalloc (1, sizeof *queue);
    queue->mName = xstrdup (name);
    queue->mLog = sbfMw_getLog (mw);

    queue->mProperties = sbfProperties_filterN (sbfMw_getProperties (mw),
                                                "queue",
                                                name,
                                                NULL);

    value = sbfKeyValue_get (queue->mProperties, "blocking");
    if (value == NULL || atoi (value) != 0)
        queue->mFlags = 0;
    else
        queue->mFlags = SBF_QUEUE_NONBLOCK;

    queue->mPool = sbfPool_create (sizeof (struct sbfQueueItemImpl));

    sbfRefCount_init (&queue->mRefCount, 1);

    sbfQueueCreate (queue);

    sbfLog_debug (queue->mLog, "creating queue %p", queue);

    return queue;
}

void
sbfQueue_destroy (sbfQueue queue)
{
    sbfLog_debug (queue->mLog, "destroying queue %p", queue);

    sbfHiResTimer_queueDestroy (queue);

    sbfQueue_addRef (queue);
    sbfQueue_enqueue (queue, NULL, NULL);
    queue->mExited = 1;

    sbfQueue_removeRef (queue);
}

const char*
sbfQueue_getName (sbfQueue queue)
{
    return queue->mName;
}

void
sbfQueue_addRef (sbfQueue queue)
{
    sbfRefCount_increment (&queue->mRefCount);
}

void
sbfQueue_removeRef (sbfQueue queue)
{
    if (!sbfRefCount_decrement (&queue->mRefCount))
        return;

    sbfLog_debug (queue->mLog, "freeing queue %p", queue);

    sbfQueueDestroy (queue);

    sbfPool_destroy (queue->mPool);

    sbfKeyValue_destroy (queue->mProperties);

    free ((void*)queue->mName);
    free (queue);
}

void
sbfQueue_enqueue (sbfQueue queue, sbfQueueCb cb, void* closure)
{
    sbfQueueItem item;

    item = sbfQueue_getItem (queue, cb, closure);
    sbfQueue_enqueueItem (queue, item);
}

void
sbfQueue_dispatch (sbfQueue queue)
{
    sbfQueueItem item;

    sbfQueue_addRef (queue);
    sbfLog_debug (queue->mLog, "queue %p entered", queue);

    for (;;)
    {
        item = sbfQueueDequeue (queue);
        if (SBF_LIKELY (item != NULL))
        {
            if (SBF_LIKELY (item->mCb != NULL))
                item->mCb (item, item->mClosure);
            sbfPool_put (item);
            if (SBF_UNLIKELY (item->mCb == NULL))
            {
                sbfQueue_removeRef (queue);
                break; /* NULL callback means destroyed */
            }
        }
        sbfHiResTimer_queueDispatch (queue);
    }

    sbfLog_debug (queue->mLog, "queue %p exited", queue);
    sbfQueue_removeRef (queue);
}

sbfQueueItem
sbfQueue_getItem (sbfQueue queue, sbfQueueCb cb, void* closure)
{
    sbfQueueItem item;

    item = sbfPool_get (queue->mPool);
    item->mCb = cb;
    item->mClosure = closure;
    return item;
}

void
sbfQueue_enqueueItem (sbfQueue queue, sbfQueueItem item)
{
    SBF_ASSERT (!queue->mExited);
    sbfQueueEnqueue (queue, item);
}

void*
sbfQueue_getItemData (sbfQueueItem item)
{
    return item->mData;
}

size_t
sbfQueue_getItemDataSize (sbfQueueItem item)
{
    return sizeof item->mData;
}
