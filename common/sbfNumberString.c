#include "sbfNumberString.h"

int64_t
sbfNumberString_toInt64 (const char* s,
                         int64_t min,
                         int64_t max,
                         const char** error)
{
    int64_t n;
    char*   endptr;

    if (s == NULL || *s == '\0')
    {
        *error = "is empty";
        return 0;
    }
    if (min > max)
    {
        *error = "has min larger than max";
        return 0;
    }

    errno = 0;
    n = strtoll (s, &endptr, 10);
    if (errno == EINVAL)
    {
        *error = "is invalid";
        return 0;
    }
    if ((errno == ERANGE && n == LLONG_MIN) || n < min)
    {
        *error = "is too small";
        return 0;
    }
    if ((errno == ERANGE && n == LLONG_MAX) || n > max)
    {
        *error = "is too large";
        return 0;
    }

    *error = NULL;
    return n;
}

double
sbfNumberString_toDouble (const char* s, const char** error)
{
    double d;
    char*  endptr;

    errno = 0;
    d = strtod (s, &endptr);
    if (errno != 0 || *endptr != '\0')
    {
        if (errno == ERANGE)
        {
            *error = "is out of range";
            return 0;
        }
        *error = "is invalid";
        return 0;
    }

    *error = NULL;
    return d;
}
