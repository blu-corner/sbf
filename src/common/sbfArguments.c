#include "sbfArguments.h"
#include "sbfArgumentsPrivate.h"

sbfArguments
sbfArguments_create (const char* options, int argc, char** argv)
{
    sbfArguments a;
    int          opt;
    u_int        i;
    char*        options0;

    xasprintf (&options0, "+%s", options);

    a = xcalloc (1, sizeof *a);

    opterr = 0;
    optind = 0;
    while ((opt = getopt (argc, argv, options0)) != -1)
    {
        if (opt == '?' || opt > UCHAR_MAX)
            goto fail;
        a->mOptions[(u_char)opt] = 1;
        if (optarg != NULL)
            a->mValues[(u_char)opt] = xstrdup (optarg);
    }
    argc -= optind;
    argv += optind;

    a->mArgumentsSize = argc;
    if (argc > 0)
    {
        a->mArguments = xcalloc (argc, sizeof *a->mArguments);
        for (i = 0; i < (u_int)argc; i++)
            a->mArguments[i] = xstrdup (argv[i]);
    }

    free (options0);
    return a;

fail:
    sbfArguments_destroy (a);
    return NULL;
}

void
sbfArguments_destroy (sbfArguments a)
{
    u_int i;

    for (i = 0; i < SBF_ARRAYSIZE (a->mOptions); i++)
        free ((void*)a->mValues[i]);

    for (i = 0; i < a->mArgumentsSize; i++)
        free ((void*)a->mArguments[i]);
    free (a->mArguments);

    free (a);
}

int
sbfArguments_hasOption (sbfArguments a, u_char c)
{
    return a->mOptions[c];
}

const char*
sbfArguments_optionValue (sbfArguments a, u_char c)
{
    if (!a->mOptions[c])
        return NULL;
    return a->mValues[c];
}

u_int
sbfArguments_size (sbfArguments a)
{
    return a->mArgumentsSize;
}

const char*
sbfArguments_get (sbfArguments a, u_int idx)
{
    if (idx >= a->mArgumentsSize)
        return NULL;
    return a->mArguments[idx];
}

void
sbfArguments_pack (int argc, char** argv, char** buf, size_t* len)
{
    int    i;
    size_t size;

    *buf = NULL;
    *len = 0;

    if (argc == 0)
        return;

    for (i = 0; i < argc; i++)
    {
        size = strlen (argv[i]) + 1;

        *buf = xrealloc (*buf, 1, *len + size);
        memcpy (*buf + *len, argv[i], size);

        *len += size;
    }
}

void
sbfArguments_unpack (char* buf, size_t len, int* argc, char*** argv)
{
    size_t size;

    *argc = 0;
    *argv = NULL;

    while (len > 0)
    {
        if (buf[len - 1] != '\0')
            break;
        size = strlen (buf) + 1;

        (*argc)++;
        *argv = xrealloc (*argv, *argc, sizeof **argv);
        (*argv)[(*argc) - 1] = xstrdup (buf);

        buf += size;
        len -= size;
    }
}

void
sbfArguments_free (int argc, char** argv)
{
    while (argc > 0)
        free (argv[--argc]);
    free (argv);
}
