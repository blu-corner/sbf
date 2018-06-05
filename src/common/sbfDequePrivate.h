#ifndef _SBF_DEQUE_PRIVATE_H_
#define _SBF_DEQUE_PRIVATE_H_

#include "sbfPool.h"

SBF_BEGIN_DECLS

struct sbfDequeItemImpl
{
    void*                          mPointer;
    TAILQ_ENTRY (sbfDequeItemImpl) mListEntry;
};
typedef struct sbfDequeItemImpl* sbfDequeItem;
typedef TAILQ_HEAD (sbfDequeItemListImpl, sbfDequeItemImpl) sbfDequeItemList;

struct sbfDequeImpl
{
    sbfPool          mPool;
    sbfDequeItemList mList;
};

SBF_END_DECLS

#endif /* _SBF_DEQUE_PRIVATE_H_ */
