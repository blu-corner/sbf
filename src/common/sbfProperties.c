#include "sbfProperties.h"

static int
sbfPropertiesLoadFile (const char* path, sbfKeyValue table, char** error)
{
    char    tmp[PATH_MAX];
    char*   copy;
    char*   next;
    FILE*   f;
    char*   line;
    size_t linelen;
    char*   s;
    char*   value;
    u_int   n = 0;
    int     used;

    f = fopen (path, "r");
    if (f == NULL)
    {
        xasprintf (error, "open(\"%s\") failed: %s", path, strerror (errno));
        return 0;
    }

    line = NULL;
	while (1)
    {
		line = fgetln(f, &linelen);
		if (ferror(f))
			goto fail;
		if (feof(f))
			break;
        n++;

        while (linelen > 0 && line[linelen - 1] == '\n')
            line[--linelen] = '\0';
        s = line;
        while (isspace ((u_char)*s))
            s++;

        if (strncmp (s, "include ", (sizeof "include ") - 1) == 0 ||
            strncmp (s, "include\t", (sizeof "include\t") - 1) == 0)
        {
            s += (sizeof "include ") - 1;
            while (isspace ((u_char)*s))
                s++;
            if (*s != '"' || s[strlen(s) - 1] != '"')
            {
                xasprintf (error, "missing \" at %s line %u", path, n);
                goto fail;
            }
            s++;
            s[strlen (s) - 1] = '\0';

            if (*s != '/')
            {
                copy = xstrdup (path);
                used = snprintf (tmp, sizeof tmp, "%s/%s", dirname (copy), s);
                free (copy);

                if (used < 0 || (size_t)used >= sizeof tmp)
                {
                    xasprintf (error, "path too long at %s line %u", path, n);
                    goto fail;
                }
                next = tmp;
            }
            else
                next = s;
            if (!sbfPropertiesLoadFile (next, table, error))
                goto fail;
            continue;
        }

        if (*s == '\0' || *s == '#')
            continue;

        value = strchr (s, '=');
        if (value == NULL || value == s)
        {
            xasprintf (error, "no key or missing = at %s line %u", path, n);
            goto fail;
        }
        *value++ = '\0';

        sbfKeyValue_put (table, s, value);
    }

    if (line != NULL)
        free (line);
    fclose (f);
    return 1;

fail:
    if (line != NULL)
        free (line);
    fclose (f);
    return 0;
}

sbfKeyValue
sbfProperties_loadDefault (char** error)
{
    sbfKeyValue table;
    char*       file;

    file = sbfPath_getConf ("sbf.properties");
    table = sbfProperties_load (file, error);
    free (file);
    return table;
}

sbfKeyValue
sbfProperties_load (const char* file, char** error)
{
    sbfKeyValue table;

    table = sbfKeyValue_create ();
    if (!sbfPropertiesLoadFile (file, table, error))
    {
        sbfKeyValue_destroy (table);
        return NULL;
    }
    return table;
}

sbfKeyValue
sbfProperties_filter (sbfKeyValue table, const char* prefix)
{
    sbfKeyValue new;
    const char* key;
    const char* value;
    void*       cookie;
    size_t      prefixlen = strlen (prefix);

    new = sbfKeyValue_create ();

    key = sbfKeyValue_first (table, &cookie);
    while (key != NULL)
    {
        value = sbfKeyValue_get (table, key);
        if (strncmp (key, prefix, prefixlen) == 0 && key[prefixlen] == '.')
            sbfKeyValue_put (new, key + prefixlen + 1, value);
        else if (sbfKeyValue_get (new, key) == NULL)
            sbfKeyValue_put (new, key, value);
        key = sbfKeyValue_next (table, &cookie);
    }

    return new;
}

sbfKeyValue
sbfProperties_filterV (sbfKeyValue table, const char* fmt, ...)
{
    sbfKeyValue new;
    va_list     ap;
    char*       prefix;

    va_start (ap, fmt);
    xvasprintf (&prefix, fmt, ap);
    va_end (ap);

    new = sbfProperties_filter (table, prefix);

    free (prefix);
    return new;
}

sbfKeyValue
sbfProperties_filterN (sbfKeyValue table, ...)
{
    sbfKeyValue new = table;
    sbfKeyValue filtered;
    va_list     ap;
    const char* prefix;

    va_start (ap, table);
    for (;;)
    {
        prefix = va_arg (ap, const char*);
        if (prefix == NULL)
            break;
        filtered = sbfProperties_filter (new, prefix);

        if (new != table)
            sbfKeyValue_destroy (new);
        new = filtered;
    }
    va_end (ap);

    return new;
}

sbfError
sbfProperties_getGroup (sbfKeyValue table,
                        const char* group,
                        sbfPropertiesGroupCb cb,
                        void* closure)
{
    size_t      grouplen = strlen (group);
    char**      list;
    u_int       listsize;
    const char* key;
    char*       copy;
    void*       cookie;
    char*       name;
    u_int       i;
    sbfKeyValue filtered;
    sbfError    error = 0;

    if (sbfKeyValue_size (table) == 0)
        return 0;

    list = xcalloc (sbfKeyValue_size (table), sizeof *list);
    listsize = 0;

    key = sbfKeyValue_first (table, &cookie);
    for (; key != NULL; key = sbfKeyValue_next (table, &cookie))
    {
        if (strncmp (key, group, grouplen) != 0 || key[grouplen] != '.')
            continue;
        copy = xstrdup (key);

        name = copy + grouplen + 1;
        name[strcspn (name, ".")] = '\0';

        for (i = 0; i < listsize; i++)
        {
            if (strcmp (name, list[i]) == 0)
                break;
        }
        if (i != listsize)
        {
            free (copy);
            continue;
        }
        list[listsize++] = xstrdup (name);

        filtered = sbfProperties_filterN (table, group, name, NULL);
        error = cb (name, filtered, closure);
        sbfKeyValue_destroy (filtered);

        free (copy);

        if (error != 0)
            break;
    }

    for (i = 0; i < listsize; i++)
        free (list[i]);
    free (list);

    return error;
}

void
sbfProperties_log (sbfLog log,
                   sbfLogLevel level,
                   const char* prefix,
                   sbfKeyValue table)
{
    const char* key;
    void*       cookie;

    key = sbfKeyValue_first (table, &cookie);
    while (key != NULL)
    {
        sbfLog_log (log,
                    level,
                    "%s%s=%s",
                    prefix,
                    key,
                    sbfKeyValue_get (table, key));
        key = sbfKeyValue_next (table, &cookie);
    }
}
