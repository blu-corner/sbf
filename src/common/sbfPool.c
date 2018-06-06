#include "sbfPool.h"

static int      gSbfPoolAtExitCalled;
static sbfPool* gSbfPoolAtExitList;
static u_int    gSbfPoolAtExitListSize;

SBF_RB_TREE (sbfPoolCount, Tree, mTreeEntry,
{
    int retval;

    retval = strcmp (lhs->mFunction, rhs->mFunction);
    if (retval != 0)
        return retval;

    if (lhs->mLine < rhs->mLine)
        return -1;
    if (lhs->mLine > rhs->mLine)
        return 1;

    if (lhs->mSize < rhs->mSize)
        return -1;
    if (lhs->mSize > rhs->mSize)
        return 1;
    return 0;
})
static sbfPoolCountTree gSbfPoolCountTree = RB_INITIALIZER (&gSbfPoolCounts);

#ifdef WIN32
// https://stackoverflow.com/questions/12853554/equivalent-of-pthread-mutex-initializer-on-windows
static sbfMutex         gSbfPoolCountMutex;
static void __cdecl sbfPoolInitializeStaticMutex(void) {
    sbfMutex_init (&gSbfPoolCountMutex, 1);
}
#pragma section(".CRT$XCU", read)
__declspec(allocate(".CRT$XCU"))
const void (__cdecl *pInitialize)(void) = sbfPoolInitializeStaticMutex;
#else
static sbfMutex         gSbfPoolCountMutex = PTHREAD_MUTEX_INITIALIZER;
#endif

static int     gSbfPoolDefaultReady;
static sbfPool gSbfPoolDefault16;
static sbfPool gSbfPoolDefault32;
static sbfPool gSbfPoolDefault64;
static sbfPool gSbfPoolDefault128;
static sbfPool gSbfPoolDefault256;
static sbfPool gSbfPoolDefault512;

static const sbfPool* gSbfDefaultPools[] = {
    &gSbfPoolDefault16,  /* 32 */
    &gSbfPoolDefault16,  /* 31 */
    &gSbfPoolDefault16,  /* 30 */
    &gSbfPoolDefault16,  /* 29 */
    &gSbfPoolDefault16,  /* 28 */
    &gSbfPoolDefault32,  /* 27 */
    &gSbfPoolDefault64,  /* 26 */
    &gSbfPoolDefault128, /* 25 */
    &gSbfPoolDefault256, /* 24 */
    &gSbfPoolDefault512, /* 23 */
};

static void
sbfPoolMakeDefault (void)
{
    gSbfPoolDefault16 = sbfPool_create (16);
    sbfPool_registerAtExit (gSbfPoolDefault16);
    gSbfPoolDefault32 = sbfPool_create (32);
    sbfPool_registerAtExit (gSbfPoolDefault32);
    gSbfPoolDefault64 = sbfPool_create (64);
    sbfPool_registerAtExit (gSbfPoolDefault64);
    gSbfPoolDefault128 = sbfPool_create (128);
    sbfPool_registerAtExit (gSbfPoolDefault128);
    gSbfPoolDefault256 = sbfPool_create (256);
    sbfPool_registerAtExit (gSbfPoolDefault256);
    gSbfPoolDefault512 = sbfPool_create (512);
    sbfPool_registerAtExit (gSbfPoolDefault512);

    gSbfPoolDefaultReady = 1;
}

static void
sbfPoolAtExit (void)
{
    u_int        i;
    sbfPoolCount count;
    sbfPoolCount count1;

    for (i = 0; i < gSbfPoolAtExitListSize; i++)
        sbfPool_destroy (gSbfPoolAtExitList[i]);
    free (gSbfPoolAtExitList);
    gSbfPoolAtExitListSize = 0;

    RB_FOREACH_SAFE (count, sbfPoolCountTreeImpl, &gSbfPoolCountTree, count1)
    {
        RB_REMOVE (sbfPoolCountTreeImpl, &gSbfPoolCountTree, count);
        free (count);
    }
}

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
sbfPool_default (size_t size)
{
    sbfPool pool;
    u_int   bits;

    if (size == 0 || size > 512)
        return NULL;

#if WIN32
    u_int leading_zero = 0;
    if (_BitScanReverse (&leading_zero, (u_int)size))
    {
        bits = 31 - leading_zero;
    }
    else
    {
        bits = 32;
    } 
#else
    bits = __builtin_clz ((u_int)size - 1);
#endif

    if (!gSbfPoolDefaultReady)
        sbfPoolMakeDefault ();

    pool = *gSbfDefaultPools[32 - bits];
    SBF_ASSERT (size <= pool->mItemSize);
    return pool;
}

sbfPool
sbfPool_create1 (size_t itemSize, const char* function, u_int line)
{
    sbfPool                 pool;
    struct sbfPoolCountImpl impl;

    pool = xcalloc (1, sizeof *pool);

    sbfSpinLock_init (&pool->mLock);

    pool->mFree = NULL;
    pool->mAvailable = NULL;

    pool->mItemSize = itemSize;
    pool->mSize = itemSize + sizeof (struct sbfPoolItemImpl);

    impl.mFunction = function;
    impl.mLine = line;
    impl.mSize = itemSize;

    sbfMutex_lock (&gSbfPoolCountMutex);
    pool->mCount = RB_FIND (sbfPoolCountTreeImpl, &gSbfPoolCountTree, &impl);
    if (pool->mCount == NULL)
    {
        pool->mCount = xcalloc (1, sizeof *pool);
        pool->mCount->mFunction = function;
        pool->mCount->mLine = line;
        pool->mCount->mPools = 0;
        pool->mCount->mSize = itemSize;
        RB_INSERT (sbfPoolCountTreeImpl, &gSbfPoolCountTree, pool->mCount);
    }
    pool->mCount->mPools++;
    sbfMutex_unlock (&gSbfPoolCountMutex);

    if (!gSbfPoolAtExitCalled)
    {
        atexit (sbfPoolAtExit);
        gSbfPoolAtExitCalled = 1;
    }

    return pool;
}

void
sbfPool_destroy (sbfPool pool)
{
    if (gSbfPoolDefaultReady &&
        (pool == gSbfPoolDefault16 || pool == gSbfPoolDefault32 ||
         pool == gSbfPoolDefault64 || pool == gSbfPoolDefault128 ||
         pool == gSbfPoolDefault256 || pool == gSbfPoolDefault512))
        return;

    sbfPoolDestroyList (pool->mFree);
    sbfPoolDestroyList (pool->mAvailable);

    sbfSpinLock_destroy (&pool->mLock);
    free (pool);
}

void
sbfPool_registerAtExit (sbfPool pool)
{
    gSbfPoolAtExitList = xrealloc (gSbfPoolAtExitList,
                                   gSbfPoolAtExitListSize + 1,
                                   sizeof *gSbfPoolAtExitList);
    gSbfPoolAtExitList[gSbfPoolAtExitListSize++] = pool;
}

void
sbfPool_showSummary (sbfLog log, sbfLogLevel level)
{
    sbfPoolCount count;
    char         tmp[64];

    sbfLog_log (log,
                level,
                "%-30s %6s%6s%11s%11s",
                "Pool -------------------------",
                "Pools",
                "Size",
                "Get",
                "New");
    sbfMutex_lock (&gSbfPoolCountMutex);
    RB_FOREACH (count, sbfPoolCountTreeImpl, &gSbfPoolCountTree)
    {
        if (count->mGets != 0)
        {
            snprintf (tmp,
                      sizeof tmp,
                      "%s:%u",
                      count->mFunction,
                      count->mLine);

            sbfLog_log (log,
                        level,
                        "%-30.30s %6u%6zu%11u%11u",
                        tmp,
                        count->mPools,
                        count->mSize,
                        count->mGets,
                        count->mNews);
        }
    }
    sbfMutex_unlock (&gSbfPoolCountMutex);
}
