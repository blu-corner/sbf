#ifndef _SBF_CACHE_FILE_PRIVATE_H_
#define _SBF_CACHE_FILE_PRIVATE_H_

SBF_BEGIN_DECLS

struct sbfCacheFileItemImpl
{
    sbfCacheFile                       mParent;
    size_t                             mOffset;

    TAILQ_ENTRY (sbfCacheFileItemImpl) mEntry;
};

struct sbfCacheFileImpl
{
    FILE*                               mFile;
    size_t                              mUsed;

    size_t                              mItemSize;
    TAILQ_HEAD (, sbfCacheFileItemImpl) mItems;
};

SBF_END_DECLS

#endif /* _SBF_CACHE_FILE_PRIVATE_H_ */
