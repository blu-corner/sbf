#include "sbfLogFile.h"
#include "sbfLogFilePrivate.h"

static const char*
sbfLogFileGetPath (const char* root, const char* fmt, va_list ap)
{
    char* tmp;
    char* path;

    xvasprintf (&tmp, fmt, ap);
    if (*tmp == '/')
        return tmp;

    xasprintf (&path, "%s/%s", root, tmp);
    free (tmp);
    return path;
}

size_t
sbfLogFile_size (void)
{
    return SBF_LOG_FILE_ENTRIES * sizeof (struct sbfLogFileEntryImpl);
}

sbfLogFile
sbfLogFile_create (char** error, const char* root, const char* fmt, ...)
{
    sbfLogFile lf;
    va_list    ap;
    char*      tmp;
    int        fd;
    size_t     size;

    lf = xcalloc (1, sizeof *lf);

    va_start (ap, fmt);
    lf->mPath = sbfLogFileGetPath (root, fmt, ap);
    va_end (ap);

    xasprintf (&tmp, "%s.tmp", lf->mPath);
    fd = open (tmp, O_RDWR|O_CREAT|O_NOATIME, 0644);
    if (fd == -1)
    {
        xasprintf (error, "open(\"%s\") failed: %s", tmp, strerror (errno));
        goto fail;
    }

    size = sbfLogFile_size ();
    if (pwrite (fd, ".", 1, size - 1) != 1)
    {
        xasprintf (error,
                   "pwrite(\"%s\", %zu) failed: %s",
                   tmp,
                   size,
                   strerror (errno));
        goto fail;
    }
    if (rename (tmp, lf->mPath) != 0)
    {
        xasprintf (error,
                   "rename(\"%s\", \"%s\") failed: %s",
                   tmp,
                   lf->mPath,
                   strerror (errno));
        goto fail;
    }

    lf->mBase = mmap (NULL, size, PROT_WRITE, MAP_SHARED|MAP_POPULATE, fd, 0);
    if (lf->mBase == MAP_FAILED)
    {
        xasprintf (error,
                   "mmap(\"%s\", %zu) failed: %s",
                   tmp,
                   size,
                   strerror (errno));
        goto fail;
    }
    lf->mNext = 0;

    close (fd);

    free (tmp);
    return lf;

fail:
    sbfLogFile_close (lf);

    if (fd != -1)
        close (fd);

    free (tmp);
    return NULL;
}

sbfLogFile
sbfLogFile_open (char** error, const char* root, const char* fmt, ...)
{
    sbfLogFile  lf;
    va_list     ap;
    int         fd;
    size_t      size;
    struct stat sb;

    lf = xcalloc (1, sizeof *lf);

    va_start (ap, fmt);
    lf->mPath = sbfLogFileGetPath (root, fmt, ap);
    va_end (ap);

    fd = open (lf->mPath, O_RDWR|O_NOATIME);
    if (fd == -1)
    {
        xasprintf (error,
                   "open(\"%s\") failed: %s",
                   lf->mPath,
                   strerror (errno));
        goto fail;
    }

    size = sbfLogFile_size ();
    if (fstat (fd, &sb) != 0)
    {
        xasprintf (error,
                   "fstat(\"%s\") failed: %s",
                   lf->mPath,
                   strerror (errno));
        goto fail;
    }
    if (sb.st_size != (off_t)size)
    {
        xasprintf (error,
                   "%s is the wrong size (size %zu, expected %zu)",
                   lf->mPath,
                   sb.st_size,
                   size);
        goto fail;
    }

    lf->mBase = mmap (NULL, size, PROT_READ, MAP_SHARED, fd, 0);
    if (lf->mBase == MAP_FAILED)
    {
        xasprintf (error,
                   "mmap(\"%s\", %zu) failed: %s",
                   lf->mPath,
                   size,
                   strerror (errno));
        goto fail;
    }
    lf->mNext = 0;

    close (fd);

    return lf;

fail:
    sbfLogFile_close (lf);

    if (fd != -1)
        close (fd);

    return NULL;
}

void
sbfLogFile_close (sbfLogFile lf)
{
    if (lf->mBase != NULL)
        munmap (lf->mBase, sbfLogFile_size ());

    free ((void*)lf->mPath);
    free (lf);
}

sbfLogFileEntry
sbfLogFile_get (sbfLogFile lf)
{
    sbfLogFileEntry lfe;
    u_int           number;

    number = __sync_fetch_and_add (&lf->mNext, 1);

    lfe = &lf->mBase[number % SBF_LOG_FILE_ENTRIES];
    lfe->mNumber = number;
    return lfe;
}

void
sbfLogFile_flush (sbfLogFile lf, sbfLogFileEntry lfe)
{
    __sync_synchronize ();
}

sbfLogFileEntry
sbfLogFile_read (sbfLogFile lf, u_int* number)
{
    sbfLogFileEntry lfe;

    __sync_synchronize ();

    lfe = &lf->mBase[*number % SBF_LOG_FILE_ENTRIES];
    if (lfe->mTime.tv_sec == 0 || lfe->mNumber < *number)
        return NULL;
    (*number)++;
    return lfe;
}
