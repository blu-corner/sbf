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

SBF_RB_TREE (sbfKeyValueItem, Tree, mTreeEntry,
{
    return strcmp (lhs->mKey, rhs->mKey);
})

struct sbfKeyValueImpl
{
    sbfKeyValueItemTree mTree;
    u_int               mSize;
};

SBF_END_DECLS

#endif /* _SBF_KEY_VALUE_PRIVATE_H_ */
