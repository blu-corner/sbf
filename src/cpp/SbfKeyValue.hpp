#pragma once

#include "sbfKeyValue.h"

namespace neueda
{

class SbfKeyValue {
public:
    SbfKeyValue (void)
    {
        mValue = sbfKeyValue_create ();
    }

    virtual ~SbfKeyValue ()
    {
        if (getHandle () != NULL)
            sbfKeyValue_destroy (getHandle ());
    }

    virtual sbfKeyValue getHandle ()
    {
        return mValue;
    }

    virtual sbfKeyValue copy ()
    {
        return sbfKeyValue_copy (getHandle ());
    }

    virtual unsigned int size ()
    {
        return sbfKeyValue_size (getHandle ());
    }

    virtual const char* get (const char* key)
    {
        return sbfKeyValue_get (getHandle (), key);
    }

    virtual const char* getV (const char* fmt, ...)
    {
        va_list     ap;
        char*       key;
        const char* value;

        va_start (ap, fmt);
        xvasprintf (&key, fmt, ap);
        va_end (ap);

        value = get (key);
        free (key);
        return value;
    }

    virtual void put (const char* key, const char* value)
    {
        sbfKeyValue_put (getHandle(), key, value);
    }

    virtual void remove (const char* key)
    {
        sbfKeyValue_remove (getHandle(), key);
    }

    virtual const char* first (void** cookie)
    {
        return sbfKeyValue_first (getHandle(), cookie);
    }

    virtual const char* next (void** cookie)
    {
        return sbfKeyValue_next (getHandle(), cookie);
    }

protected:
    sbfKeyValue mValue;
};

}
