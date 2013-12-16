#ifndef _SBF_POOL_H_
#error "must be included from sbfPool.h"
#endif

struct sbfPoolItemImpl
{
    sbfPool                      mPool;
    LIST_ENTRY (sbfPoolItemImpl) mEntry;
};
typedef struct sbfPoolItemImpl* sbfPoolItem;

struct sbfPoolImpl
{
    size_t                        mItemSize;
    size_t                        mSize;

    sbfSpinLock                   mLock;
    LIST_HEAD (, sbfPoolItemImpl) mList;
};

static SBF_INLINE sbfPoolItem
sbfPoolNew (sbfPool pool)
{
    sbfPoolItem item;

    item = (sbfPoolItem)xmalloc (pool->mSize);
    item->mPool = pool;
    memset (&item->mEntry, 0, sizeof item->mEntry);
    return item;
}

static SBF_INLINE sbfPoolItem
sbfPoolNewZero (sbfPool pool)
{
    sbfPoolItem item;

    item = (sbfPoolItem)xcalloc (1, pool->mSize);
    item->mPool = pool;
    return item;
}

static SBF_INLINE size_t
sbfPool_getItemSize (sbfPool pool)
{
    return pool->mItemSize;
}

static SBF_INLINE void*
sbfPool_get (sbfPool pool)
{
    sbfPoolItem item;

    sbfSpinLock_lock (&pool->mLock);
    item = LIST_FIRST (&pool->mList);
    if (item != NULL)
    {
        LIST_REMOVE (item, mEntry);
        sbfSpinLock_unlock (&pool->mLock);
        return item + 1;
    }
    sbfSpinLock_unlock (&pool->mLock);
    return sbfPoolNew (pool) + 1;
}

static SBF_INLINE void*
sbfPool_getZero (sbfPool pool)
{
    sbfPoolItem item;

    sbfSpinLock_lock (&pool->mLock);
    item = LIST_FIRST (&pool->mList);
    if (item != NULL)
    {
        LIST_REMOVE (item, mEntry);
        sbfSpinLock_unlock (&pool->mLock);
        memset (item + 1, 0, pool->mItemSize);
        return item + 1;
    }
    sbfSpinLock_unlock (&pool->mLock);
    return sbfPoolNewZero (pool) + 1;
}

static SBF_INLINE void
sbfPool_put (void* data)
{
    sbfPoolItem item = (sbfPoolItem)data - 1;
    sbfPool     pool = item->mPool;

    sbfSpinLock_lock (&pool->mLock);
    LIST_INSERT_HEAD (&pool->mList, item, mEntry);
    sbfSpinLock_unlock (&pool->mLock);
}
