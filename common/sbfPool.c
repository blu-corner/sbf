#include "sbfPool.h"

sbfPool
sbfPool_create (size_t itemSize)
{
    sbfPool pool;

    pool = xcalloc (1, sizeof *pool);

    sbfSpinLock_init (&pool->mLock);
    SLIST_INIT (&pool->mList);

    pool->mItemSize = itemSize;
    pool->mSize = itemSize + sizeof (struct sbfPoolItemImpl);

    return pool;
}

void
sbfPool_destroy (sbfPool pool)
{
    sbfPoolItem item;
    sbfPoolItem item1;

    SLIST_FOREACH_SAFE (item, &pool->mList, mEntry, item1)
        free (item);

    sbfSpinLock_destroy (&pool->mLock);
    free (pool);
}
