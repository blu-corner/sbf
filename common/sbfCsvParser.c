#include "sbfCsvParser.h"

static SBF_INLINE void
sbfCsvParserExpand (char** buf, size_t* len, size_t wanted)
{
    (*len) += wanted;
    *buf = xrealloc (*buf, *len, 1);
}

static SBF_INLINE void
sbfCsvParserAdd (char*** fields, u_int* nfields, char* field)
{
    (*nfields)++;
    *fields = xrealloc (*fields, *nfields, sizeof **fields);
    (*fields)[(*nfields) - 1] = field;
}

sbfError
sbfCsvParser_next (FILE* f,
                   char*** fields,
                   u_int* nfields,
                   u_char separator,
                   u_int* line)
{
    char*  buf;
    size_t len;
    size_t off;
    u_char c;
    char*  next;
    size_t nextlen;
    int    quoted;
    int    done;
    int    comma;
    int    saved_errno;

    if (line != NULL)
        *line = 0;
    do
    {
        buf = fgetln (f, &len);
        if (buf == NULL)
        {
            saved_errno = errno;
            if (feof (f))
                return ENOENT;
           return saved_errno;
        }
        if (line != NULL)
            (*line)++;
        while (len > 0 && (*buf == ' ' || *buf == '\t'))
        {
            buf++;
            len--;
        }
        if (len > 0 && buf[len - 1] == '\n')
            len--;
    }
    while (len == 0);

    *fields = NULL;
    *nfields = 0;

    off = 0;
    for (;;)
    {
        next = NULL;
        nextlen = 0;

        while (off != len && (buf[off] == ' ' || buf[off] == '\t'))
            off++;
        if (off == len)
        {
            sbfCsvParserAdd (fields, nfields, xstrdup (""));
            break;
        }

        quoted = buf[off] == '"';
        if (quoted)
            off++;

        comma = done = 0;
        while (off != len)
        {
            c = buf[off++];

            if (!quoted && c == separator)
            {
                while (nextlen > 0 && next[nextlen - 1] == ' ')
                    nextlen--;
                comma = 1;
                break;
            }
            if (quoted && c == '"')
            {
                if (off == len)
                    break;
                if (buf[off] != '"')
                {
                    while (off != len && (buf[off] == ' ' || buf[off] == '\t'))
                        off++;
                    if (off != len && buf[off++] != separator)
                        goto fail;
                    if (off != len)
                        comma = 1;
                    break;
                }
                else
                    off++;
            }

            sbfCsvParserExpand (&next, &nextlen, 1);
            next[nextlen - 1] = c;
        }

        sbfCsvParserExpand (&next, &nextlen, 1);
        next[nextlen - 1] = '\0';
        sbfCsvParserAdd (fields, nfields, next);

        if (!comma)
            break;
    }

    return 0;

fail:
    if (next != NULL)
        free (next);
    sbfCsvParser_free (*fields, *nfields);
    return EINVAL;
}

void
sbfCsvParser_free (char** fields, u_int nfields)
{
    u_int i;

    for (i = 0; i < nfields; i++)
        free (fields[i]);
    free (fields);
}
