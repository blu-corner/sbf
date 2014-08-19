#ifndef _SBF_QUEUE_PRIVATE_H_
#error "must be included from sbfQueuePrivate.h"
#endif

/*
 * Based on code from http://tinyurl.com/44c6dqd with a futex added for
 * blocking.
 */

#ifndef SBF_QUEUE_FUNCTIONS

#define SBF_QUEUE_DECL                            \
    int                               mSemaphore; \
    struct sbfQueueItemImpl* volatile mHead;      \
    struct sbfQueueItemImpl*          mTail;      \
    struct sbfQueueItemImpl           mEmpty;

#define SBF_QUEUE_ITEM_DECL                       \
    struct sbfQueueItemImpl* volatile mNext;

#else /* SBF_QUEUE_FUNCTIONS */

static SBF_INLINE void
sbfQueueCreate (sbfQueue queue)
{
    queue->mSemaphore = 0; /* 0 empty, -1 empty with waiter, >0 not empty */
    queue->mHead = &queue->mEmpty;
    queue->mTail = &queue->mEmpty;
    queue->mEmpty.mNext = NULL;
}

static SBF_INLINE void
sbfQueueWake (sbfQueue queue)
{
    if (SBF_QUEUE_BLOCKING (queue))
        futex (&queue->mSemaphore, FUTEX_WAKE, INT_MAX, NULL, NULL, 0);
}

static SBF_INLINE void
sbfQueueDestroy (sbfQueue queue)
{
    sbfQueueItem item;
    sbfQueueItem next;

    item = queue->mHead;
    while (item != NULL)
    {
        next = item->mNext;
        sbfPool_put (item);
        item = next;
    }
}

static SBF_INLINE void
sbfQueueEnqueue (sbfQueue queue, sbfQueueItem item)
{
    sbfQueueItem last;

    item->mNext = NULL;
    last = __sync_lock_test_and_set (&queue->mHead, item);
    last->mNext = item;

    if (SBF_QUEUE_BLOCKING (queue))
    {
        if (__sync_bool_compare_and_swap (&queue->mSemaphore, -1, 1))
            futex (&queue->mSemaphore, FUTEX_WAKE, INT_MAX, NULL, NULL, 0);
        else
            __sync_fetch_and_add (&queue->mSemaphore, 1);
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
    sbfQueueEnqueue (queue, &queue->mEmpty);

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
    int          s;

    if (!SBF_QUEUE_BLOCKING (queue))
        return sbfQueueNext (queue);

    while (!queue->mDestroyed)
    {
        s = __sync_sub_and_fetch (&queue->mSemaphore, 1);
        SBF_ASSERT (s >= -1);
        if (s != -1)
            break;
        while (futex (&queue->mSemaphore, FUTEX_WAIT, -1, NULL, NULL, 0) != 0)
        {
            if (errno == EWOULDBLOCK)
                break;
        }
    }

    /*
     * If the semaphore was raised, an item should be removed, so wait until it
     * is there. This is alright because there can only be one consumer, so
     * nobody else can remove from the queue.
     */
    while ((item = sbfQueueNext (queue)) == NULL && !queue->mDestroyed)
        ;
    return item;
}

#endif /* SBF_QUEUE_FUNCTIONS */
