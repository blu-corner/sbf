#ifndef _SBF_POOL_H_
#error "must be included from sbfPool.h"
#endif

struct sbfPoolCountImpl
{
    const char*                 mFunction;
    u_int                       mLine;
    u_int                       mPools;
    size_t                      mSize;

    u_int                       mGets;
    u_int                       mPuts;
    u_int                       mNews;

    RB_ENTRY (sbfPoolCountImpl) mTreeEntry;
};
typedef struct sbfPoolCountImpl* sbfPoolCount;

struct sbfPoolItemImpl
{
    sbfPool                 mPool;

    struct sbfPoolItemImpl* mNext;
};
typedef struct sbfPoolItemImpl* sbfPoolItem;

struct sbfPoolImpl
{
    size_t                    mItemSize;
    size_t                    mSize;

    sbfSpinLock               mLock;
    struct sbfPoolItemImpl*   mAvailable;
    struct sbfPoolItemImpl*   mFree;

    sbfPoolCount              mCount;

    TAILQ_ENTRY (sbfPoolImpl) mListEntry;
    int                       mAtExit;
};

static SBF_INLINE sbfPoolItem
sbfPoolNew (sbfPool pool)
{
    sbfPoolItem item;

#ifdef WIN32
    InterlockedIncrement (&pool->mCount->mNews);
#else
    __sync_fetch_and_add (&pool->mCount->mNews, 1);
#endif

    item = (sbfPoolItem)xmalloc (pool->mSize);
    item->mPool = pool;
    item->mNext = NULL;
    return item;
}

static SBF_INLINE sbfPoolItem
sbfPoolNewZero (sbfPool pool)
{
    sbfPoolItem item;

#ifdef WIN32
    InterlockedIncrement (&pool->mCount->mNews);
#else
    __sync_fetch_and_add (&pool->mCount->mNews, 1);
#endif

    item = (sbfPoolItem)xcalloc (1, pool->mSize);
    item->mPool = pool;
    item->mNext = NULL;
    return item;
}

static SBF_INLINE sbfPoolItem
sbfPoolNextItem (sbfPool pool)
{
    sbfPoolItem item;
    uint64_t    value;

    sbfSpinLock_lock (&pool->mLock);

    item = pool->mAvailable;
    if (item == NULL)
    {
        value = sbfAtomic_swap ((volatile int64_t*)&pool->mFree,
                             (int64_t)NULL);
        item = (sbfPoolItem)value;
    }
    if (item != NULL)
        pool->mAvailable = item->mNext;

    sbfSpinLock_unlock (&pool->mLock);
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

#ifdef WIN32
    InterlockedIncrement (&pool->mCount->mGets);
#else
    __sync_fetch_and_add (&pool->mCount->mGets, 1);
#endif

    item = sbfPoolNextItem (pool);
    if (item == NULL)
        item = sbfPoolNew (pool);
    return item + 1;
}

static SBF_INLINE void*
sbfPool_getZero (sbfPool pool)
{
    sbfPoolItem item;

#ifdef WIN32
    InterlockedIncrement (&pool->mCount->mGets);
#else
    __sync_fetch_and_add (&pool->mCount->mGets, 1);
#endif

    item = sbfPoolNextItem (pool);
    if (item == NULL)
        item = sbfPoolNewZero (pool);
    else
        memset (item + 1, 0, pool->mItemSize);
    return item + 1;
}

static SBF_INLINE void
sbfPool_put (void* data)
{
    sbfPoolItem item = (sbfPoolItem)data - 1;
    sbfPool     pool = item->mPool;

#ifdef WIN32
    InterlockedIncrement (&pool->mCount->mPuts);
#else
    __sync_fetch_and_add (&pool->mCount->mPuts, 1);
#endif

    do
        item->mNext = pool->mFree;
    while (!sbfAtomic_compareAndSwap ((volatile int64_t*)&pool->mFree,
                                      (int64_t)item->mNext,
                                      (int64_t)item));
}
