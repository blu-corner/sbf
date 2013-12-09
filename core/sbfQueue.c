#include "sbfQueue.h"
#include "sbfQueuePrivate.h"

sbfQueue
sbfQueue_create (int flags)
{
    sbfQueue queue;

    queue = xcalloc (1, sizeof *queue);
    queue->mPool = sbfPool_create (sizeof (struct sbfQueueItemImpl));
    TAILQ_INIT (&queue->mItems);

    queue->mDestroyed = 0;
    sbfRefCount_init (&queue->mRefCount, 1);

    pthread_mutex_init (&queue->mMutex, NULL);
    pthread_cond_init (&queue->mCond, NULL);

    sbfLog_debug ("creating %p", queue);

    return queue;
}

void
sbfQueue_destroy (sbfQueue queue)
{
    sbfLog_debug ("destroying %p", queue);

    queue->mDestroyed = 1;
    pthread_cond_broadcast (&queue->mCond);

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
    sbfQueueItem item1;

    if (!sbfRefCount_decrement (&queue->mRefCount))
        return;

    TAILQ_FOREACH_SAFE (item, &queue->mItems, mEntry, item1)
    {
        TAILQ_REMOVE (&queue->mItems, item, mEntry);
        sbfPool_put (item);
    }

    pthread_cond_destroy (&queue->mCond);
    pthread_mutex_destroy (&queue->mMutex);

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

        pthread_mutex_lock (&queue->mMutex);
        while (!queue->mDestroyed &&
               (item = TAILQ_FIRST (&queue->mItems)) == NULL)
            pthread_cond_wait (&queue->mCond, &queue->mMutex);
        if (item != NULL)
            TAILQ_REMOVE (&queue->mItems, item, mEntry);
        pthread_mutex_unlock (&queue->mMutex);

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
    pthread_mutex_lock (&queue->mMutex);
    TAILQ_INSERT_TAIL (&queue->mItems, item, mEntry);
    pthread_cond_signal (&queue->mCond);
    pthread_mutex_unlock (&queue->mMutex);
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
