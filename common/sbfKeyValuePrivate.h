#ifndef _SBF_KEY_VALUE_PRIVATE_H_
#define _SBF_KEY_VALUE_PRIVATE_H_

SBF_BEGIN_DECLS

struct sbfKeyValueItemImpl
{
    const char*                    mKey;
    size_t                         mKeySize;

    RB_ENTRY (sbfKeyValueItemImpl) mTreeEntry;
};
typedef struct sbfKeyValueItemImpl* sbfKeyValueItem;

RB_HEAD (sbfKeyValueItemTreeImpl, sbfKeyValueItemImpl);
typedef struct sbfKeyValueItemTreeImpl sbfKeyValueItemTree;

static __inline int
sbfKeyValueItemCmp (sbfKeyValueItem lhs, sbfKeyValueItem rhs)
{
    return strcmp (lhs->mKey, rhs->mKey);
}
RB_GENERATE_STATIC (sbfKeyValueItemTreeImpl,
                    sbfKeyValueItemImpl,
                    mTreeEntry,
                    sbfKeyValueItemCmp)

struct sbfKeyValueImpl
{
    sbfKeyValueItemTree mTree;
};

SBF_END_DECLS

#endif /* _SBF_KEY_VALUE_PRIVATE_H_ */
