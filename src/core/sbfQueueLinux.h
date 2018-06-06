#ifndef _SBF_QUEUE_PRIVATE_H_
#error "must be included from sbfQueuePrivate.h"
#endif

/*
 * Based on code from http://tinyurl.com/44c6dqd with a futex added for
 * blocking.
 */

#ifndef SBF_QUEUE_FUNCTIONS

#define SBF_QUEUE_DECL                            \
    volatile int                      mWaiting;   \
    struct sbfQueueItemImpl* volatile mHead;      \
    struct sbfQueueItemImpl*          mTail;      \
    struct sbfQueueItemImpl           mEmpty;

#define SBF_QUEUE_ITEM_DECL                       \
    struct sbfQueueItemImpl* volatile mNext;

#else /* SBF_QUEUE_FUNCTIONS */

static SBF_INLINE void
sbfQueueCreate (sbfQueue queue)
{
    queue->mWaiting = 0;
    queue->mHead = &queue->mEmpty;
    queue->mTail = &queue->mEmpty;
    queue->mEmpty.mNext = NULL;
}

static SBF_INLINE void
sbfQueueDestroy (sbfQueue queue)
{
    sbfQueueItem item;
    sbfQueueItem next;

    item = queue->mHead;
    while (item != NULL && item != &queue->mEmpty)
    {
        next = item->mNext;
        sbfPool_put (item);
        item = next;
    }
}

static SBF_INLINE void
sbfQueueEnqueue1 (sbfQueue queue, sbfQueueItem item)
{
    sbfQueueItem last;

    item->mNext = NULL;
    last = __sync_lock_test_and_set (&queue->mHead, item);
    last->mNext = item;
}

static SBF_INLINE void
sbfQueueEnqueue (sbfQueue queue, sbfQueueItem item)
{
    sbfQueueEnqueue1 (queue, item);

    if (SBF_UNLIKELY (SBF_QUEUE_BLOCKING (queue)))
    {
        /*
         * If we manage to set the waiting flag from 1 to 0, then the queue is
         * waiting and we are elected to wake it up.
         */
        if (__sync_bool_compare_and_swap (&queue->mWaiting, 1, 0))
            futex (&queue->mWaiting, FUTEX_WAKE, INT_MAX, NULL, NULL, 0);
    }
}

static SBF_INLINE sbfQueueItem
sbfQueueNext (sbfQueue queue)
{
    sbfQueueItem tail;
    sbfQueueItem next;
    sbfQueueItem head;

    tail = queue->mTail;
    next = tail->mNext;
    if (tail == &queue->mEmpty)
    {
        if (next == NULL)
            return NULL;
        queue->mTail = next;
        tail = next;
        next = next->mNext;
    }
    if (next != NULL)
    {
        queue->mTail = next;
        return tail;
    }

    head = queue->mHead;
    if (tail != head)
        return NULL;
    sbfQueueEnqueue1 (queue, &queue->mEmpty);

    next = tail->mNext;
    if (next)
    {
        queue->mTail = next;
        return tail;
    }
    return NULL;
}

static SBF_INLINE sbfQueueItem
sbfQueueDequeue (sbfQueue queue)
{
    sbfQueueItem item;

    if (SBF_LIKELY (!SBF_QUEUE_BLOCKING (queue)))
        return sbfQueueNext (queue);

    for (;;)
    {
        item = sbfQueueNext (queue);
        if (SBF_LIKELY (item != NULL))
            return item;

        /* Set the waiting flag, using an atomic to get a barrier. */
        __sync_lock_test_and_set (&queue->mWaiting, 1);

        /*
         * Another thread could have added an item and tested the waiting flag
         * before we set it to 1. So check again before we actually sleep.
         */
        item = sbfQueueNext (queue);
        if (SBF_UNLIKELY (item != NULL))
        {
            /* No point in a wakeup now so set the flag back. */
            __sync_lock_test_and_set (&queue->mWaiting, 0);
            return item;
        }

        /*
         * If another thread changed the waiting flag after we set it to 1 then
         * it'll be 0 now and futex will return EWOULDBLOCK.
         */
        while (futex (&queue->mWaiting, FUTEX_WAIT, 1, NULL, NULL, 0) != 0)
        {
            if (errno == EWOULDBLOCK)
                break;
        }
    }
}

#endif /* SBF_QUEUE_FUNCTIONS */
