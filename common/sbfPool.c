#include "sbfPool.h"

static SBF_INLINE void
sbfPoolDestroyList (sbfPoolItem item)
{
    sbfPoolItem next;

    while (item != NULL)
    {
        next = item->mNext;
        free (item);
        item = next;
    }
}

sbfPool
sbfPool_create (size_t itemSize)
{
    sbfPool pool;

    pool = xcalloc (1, sizeof *pool);

    sbfSpinLock_init (&pool->mLock);

    pool->mFree = NULL;
    pool->mAvailable = NULL;

    pool->mItemSize = itemSize;
    pool->mSize = itemSize + sizeof (struct sbfPoolItemImpl);

    return pool;
}

void
sbfPool_destroy (sbfPool pool)
{
    sbfPoolDestroyList (pool->mFree);
    sbfPoolDestroyList (pool->mAvailable);

    sbfSpinLock_destroy (&pool->mLock);
    free (pool);
}
