#include "sbfQueue.h"
#include "sbfQueuePrivate.h"

sbfQueue
sbfQueue_create (int flags)
{
    sbfQueue queue;

    queue = xcalloc (1, sizeof *queue);
    queue->mPool = sbfPool_create (sizeof (struct sbfQueueItemImpl));
    SIMPLEQ_INIT (&queue->mItems);

    queue->mDestroyed = 0;
    sbfRefCount_init (&queue->mRefCount, 1);

    sbfMutex_init (&queue->mMutex, 0);
    sbfCondVar_init (&queue->mCondVar);

    sbfLog_debug ("creating %p", queue);

    return queue;
}

void
sbfQueue_destroy (sbfQueue queue)
{
    sbfLog_debug ("destroying %p", queue);

    queue->mDestroyed = 1;
    sbfCondVar_broadcast (&queue->mCondVar);

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
    sbfQueueItem item;

    if (!sbfRefCount_decrement (&queue->mRefCount))
        return;

    while ((item = SIMPLEQ_FIRST (&queue->mItems)) != NULL)
    {
        SIMPLEQ_REMOVE_HEAD (&queue->mItems, mEntry);
        sbfPool_put (item);
    }

    sbfCondVar_destroy (&queue->mCondVar);
    sbfMutex_destroy (&queue->mMutex);

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
        item = NULL;

        sbfMutex_lock (&queue->mMutex);
        while (!queue->mDestroyed &&
               (item = SIMPLEQ_FIRST (&queue->mItems)) == NULL)
            sbfCondVar_wait (&queue->mCondVar, &queue->mMutex);
        if (item != NULL)
            SIMPLEQ_REMOVE_HEAD (&queue->mItems, mEntry);
        sbfMutex_unlock (&queue->mMutex);

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
    sbfMutex_lock (&queue->mMutex);
    SIMPLEQ_INSERT_TAIL (&queue->mItems, item, mEntry);
    sbfCondVar_signal (&queue->mCondVar);
    sbfMutex_unlock (&queue->mMutex);
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
