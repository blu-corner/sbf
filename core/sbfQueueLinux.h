#ifndef _SBF_QUEUE_PRIVATE_H_
#error "must be included from sbfQueuePrivate.h"
#endif

/* Based on code from http://tinyurl.com/44c6dqd - MPSC lock free queue. */

#ifndef SBF_QUEUE_FUNCTIONS

#define SBF_QUEUE_DECL                          \
    struct sbfQueueItemImpl* volatile mHead;	\
    struct sbfQueueItemImpl*          mTail;	\
    struct sbfQueueItemImpl           mEmpty;

#define SBF_QUEUE_ITEM_DECL                     \
    struct sbfQueueItemImpl* volatile mNext;

#else /* SBF_QUEUE_FUNCTIONS */

static SBF_INLINE void
SBF_QUEUE_CREATE(sbfQueue queue)
{
    queue->mHead = &queue->mEmpty;
    queue->mTail = &queue->mEmpty;
    queue->mEmpty.mNext = NULL;
}

static SBF_INLINE void
SBF_QUEUE_WAKE (sbfQueue queue)
{
}

static SBF_INLINE void
SBF_QUEUE_DESTROY(sbfQueue queue)
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
SBF_QUEUE_ENQUEUE(sbfQueue queue, sbfQueueItem item)
{
    sbfQueueItem last;

    item->mNext = NULL;
    last  = __sync_lock_test_and_set (&queue->mHead, item);
    last->mNext = item;
}

static SBF_INLINE sbfQueueItem
SBF_QUEUE_DEQUEUE(sbfQueue queue)
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
    SBF_QUEUE_ENQUEUE (queue, &queue->mEmpty);

    next = tail->mNext;
    if (next)
    {
        queue->mTail = next;
        return tail;
    }
    return NULL;
}

#endif /* SBF_QUEUE_FUNCTIONS */
