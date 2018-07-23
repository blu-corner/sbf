#pragma once

#include "sbfKeyValue.h"

class SbfKeyValue {
public:
    SbfKeyValue (void)
    {
        mValue = sbfKeyValue_create (void);
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

    virtual const copy ()
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
        return sbfKeyValue_getV (getHandle (), __VA_ARGS__);
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
