#include "sbfQueue.h"
#include "sbfQueuePrivate.h"

sbfQueue
sbfQueue_create (int flags)
{
    sbfQueue queue;

    queue = xcalloc (1, sizeof *queue);
    queue->mFlags = flags;
    queue->mPool = sbfPool_create (sizeof (struct sbfQueueItemImpl));

    queue->mDestroyed = 0;
    sbfRefCount_init (&queue->mRefCount, 1);

    SBF_QUEUE_CREATE (queue);

    sbfLog_debug ("creating %p", queue);

    return queue;
}

void
sbfQueue_destroy (sbfQueue queue)
{
    sbfLog_debug ("destroying %p", queue);

    queue->mDestroyed = 1;
    SBF_QUEUE_WAKE (queue);

    sbfQueue_removeRef (queue);
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

    SBF_QUEUE_DESTROY (queue);

    sbfPool_destroy (queue->mPool);
    free (queue);
}

void
sbfQueue_enqueue (sbfQueue queue, sbfQueueCb cb, void* closure)
{
    sbfQueueItem item;

    if (queue->mDestroyed)
        return;

    item = sbfQueue_getItem (queue, cb, closure);
    sbfQueue_enqueueItem (queue, item);
}

void
sbfQueue_dispatch (sbfQueue queue)
{
    sbfQueueItem item;

    if (queue->mDestroyed)
        return;

    sbfLog_debug ("%p entered", queue);
    sbfQueue_addRef (queue);

    while (!queue->mDestroyed)
    {
        item = SBF_QUEUE_DEQUEUE (queue);
        if (item != NULL)
        {
            item->mCb (item, item->mClosure);
            sbfPool_put (item);
        }
    }

    sbfQueue_removeRef (queue);
    sbfLog_debug ("%p exited", queue);
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
    SBF_QUEUE_ENQUEUE(queue, item);
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
