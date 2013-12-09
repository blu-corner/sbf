#ifndef _SBF_POOL_H_
#error "must be included from sbfPool.h"
#endif

struct sbfPoolItemImpl
{
    sbfPool                       mPool;
    SLIST_ENTRY (sbfPoolItemImpl) mEntry;
};
typedef struct sbfPoolItemImpl* sbfPoolItem;

struct sbfPoolImpl
{
    size_t                         mItemSize;
    size_t                         mSize;

    pthread_spinlock_t             mLock;
    SLIST_HEAD (, sbfPoolItemImpl) mList;
};

static inline sbfPoolItem
sbfPoolNew (sbfPool pool)
{
    sbfPoolItem item;

    item = (sbfPoolItem)xmalloc (pool->mSize);
    item->mPool = pool;
    memset (&item->mEntry, 0, sizeof item->mEntry);
    return item;
}

static inline sbfPoolItem
sbfPoolNewZero (sbfPool pool)
{
    sbfPoolItem item;

    item = (sbfPoolItem)xcalloc (1, pool->mSize);
    item->mPool = pool;
    return item;
}

static inline size_t
sbfPool_getItemSize (sbfPool pool)
{
    return pool->mItemSize;
}

static inline void*
sbfPool_get (sbfPool pool)
{
    sbfPoolItem item;

    pthread_spin_lock (&pool->mLock);
    item = SLIST_FIRST (&pool->mList);
    if (item != NULL)
    {
        SLIST_REMOVE_HEAD (&pool->mList, mEntry);
        pthread_spin_unlock (&pool->mLock);
        return item + 1;
    }
    pthread_spin_unlock (&pool->mLock);
    return sbfPoolNew (pool) + 1;
}

static inline void*
sbfPool_getZero (sbfPool pool)
{
    sbfPoolItem item;

    pthread_spin_lock (&pool->mLock);
    item = SLIST_FIRST (&pool->mList);
    if (item != NULL)
    {
        SLIST_REMOVE_HEAD (&pool->mList, mEntry);
        pthread_spin_unlock (&pool->mLock);
        memset (item + 1, 0, pool->mItemSize);
        return item + 1;
    }
    pthread_spin_unlock (&pool->mLock);
    return sbfPoolNewZero (pool) + 1;
}

static inline void
sbfPool_put (void* data)
{
    sbfPoolItem item = (sbfPoolItem)data - 1;
    sbfPool     pool = item->mPool;

    pthread_spin_lock (&pool->mLock);
    SLIST_INSERT_HEAD (&pool->mList, item, mEntry);
    pthread_spin_unlock (&pool->mLock);
}
