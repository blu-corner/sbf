#include "sbfPath.h"

const char*
sbfPath_root (void)
{
    static const char* path;

    if (path == NULL)
    {
        path = getenv ("SBF_PATH");
        if (path == NULL || *path == '\0')
            path = ".";
    }
    return path;
}

char*
sbfPath_get (const char* fmt, ...)
{
    va_list ap;
    char*   path;

    va_start (ap, fmt);
    path = sbfPath_getV (fmt, ap);
    va_end (ap);

    return path;
}

char*
sbfPath_getV (const char* fmt, va_list ap)
{
    char* tmp;
    char* path;

    xvasprintf (&tmp, fmt, ap);
    if (*tmp == '/')
        return tmp;

    xasprintf (&path, "%s/%s", sbfPath_root (), tmp);
    free (tmp);

    return path;
}

char*
sbfPath_getConf (const char* fmt, ...)
{
    va_list     ap;
    const char* env;
    char*       tmp;
    char*       path;

    va_start (ap, fmt);

    env = getenv ("SBF_CONFIG");
    if (env == NULL || *env == '\0')
        SBF_FATAL ("SBF_CONFIG not set");

    xvasprintf (&tmp, fmt, ap);
    va_end (ap);

    path = sbfPath_get ("%s/%s", env, tmp);
    free (tmp);

    return path;
}

