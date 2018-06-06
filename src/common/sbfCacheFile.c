#include "sbfCacheFile.h"
#include "sbfCacheFilePrivate.h"

static SBF_INLINE sbfCacheFileItem
sbfCacheFileNewItem (sbfCacheFile file, size_t offset)
{
    sbfCacheFileItem item;

    item = xcalloc (1, sizeof *item);
    item->mParent = file;
    item->mOffset = offset;
    TAILQ_INSERT_TAIL (&file->mItems, item, mEntry);
    return item;
}

sbfCacheFile
sbfCacheFile_open (const char* path,
                   size_t itemSize,
                   int always_create,
                   int* created,
                   sbfCacheFileItemCb cb,
                   void* closure)
{
    sbfCacheFile     file;
    sbfCacheFileItem item;
    FILE*            f;
    struct stat      sb;
    size_t           size;
    char*            cp;
    size_t           offset;
    int              tmp;

    if (created == NULL)
        created = &tmp;

    if (!always_create)
    {
        *created = (stat (path, &sb) != 0);
        if (*created && errno != ENOENT)
            return NULL;
    } else
        *created = 1;

    if (*created)
        f = fopen (path, "w+b");
    else
        f = fopen (path, "r+b");
    if (f == NULL)
        return NULL;

    if (*created)
        size = 0;
    else
        size = sb.st_size;
    if ((size % itemSize) != 0)
        return NULL;

    file = xcalloc (1, sizeof *file);
    file->mFile = f;
    file->mUsed = size;

    file->mItemSize = itemSize;
    TAILQ_INIT (&file->mItems);

    if (!*created && size != 0 && cb != NULL)
    {
        cp = mmap (NULL, size, PROT_READ, MAP_PRIVATE, fileno (f), 0);
        if (cp == MAP_FAILED)
            goto fail;
        for (offset = 0; offset < size; offset += itemSize)
        {
            item = sbfCacheFileNewItem (file, offset);
            cb (file, item, cp + offset, itemSize, closure);
        }
        munmap (cp, size);
    }

    return file;

fail:
    sbfCacheFile_close (file);
    return NULL;
}

void
sbfCacheFile_close (sbfCacheFile file)
{
    sbfCacheFileItem item;
    sbfCacheFileItem item1;

    TAILQ_FOREACH_SAFE (item, &file->mItems, mEntry, item1)
    {
        TAILQ_REMOVE (&file->mItems, item, mEntry);
        free (item);
    }

    fclose (file->mFile);
    free (file);
}

sbfCacheFileItem
sbfCacheFile_add (sbfCacheFile file, void* itemData)
{
    sbfCacheFileItem item;

    item = sbfCacheFileNewItem (file, file->mUsed);

    file->mUsed += file->mItemSize;

    if (itemData != NULL)
        sbfCacheFile_write (item, itemData);

    return item;
}

sbfError
sbfCacheFile_write (sbfCacheFileItem item, void* itemData)
{
    sbfCacheFile file = item->mParent;

    if (fseeko (file->mFile, item->mOffset, SEEK_SET) != 0)
        return errno;
    if (fwrite (itemData, file->mItemSize, 1, file->mFile) != 1)
        return errno;
    return 0;
}

sbfError
sbfCacheFile_writeOffset (sbfCacheFileItem item,
                          size_t offset,
                          void* data,
                          size_t size)
{
    sbfCacheFile file = item->mParent;

    if (offset > file->mItemSize || offset + size > file->mItemSize)
        return EINVAL;

    if (fseeko (file->mFile, item->mOffset + offset, SEEK_SET) != 0)
        return errno;
    if (fwrite (data, size, 1, file->mFile) != 1)
        return errno;
    return 0;
}

void
sbfCacheFile_flush (sbfCacheFile file)
{
    fflush (file->mFile);
}
