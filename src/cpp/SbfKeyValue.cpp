#include "SbfKeyValue.hpp"

neueda::SbfKeyValue::SbfKeyValue ()
{
    mValue = sbfKeyValue_create ();
}

neueda::SbfKeyValue::~SbfKeyValue ()
{
    if (getHandle () != NULL)
        sbfKeyValue_destroy (getHandle ());
}

sbfKeyValue neueda::SbfKeyValue::getHandle ()
{
    return mValue;
}

sbfKeyValue neueda::SbfKeyValue::copy ()
{
    return sbfKeyValue_copy (getHandle ());
}

unsigned int neueda::SbfKeyValue::size ()
{
    return sbfKeyValue_size (getHandle ());
}

const char* neueda::SbfKeyValue::get (const char* key)
{
    return sbfKeyValue_get (getHandle (), key);
}

const char* neueda::SbfKeyValue::getV (const char* fmt, ...)
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

void neueda::SbfKeyValue::put (const char* key, const char* value)
{
    sbfKeyValue_put (getHandle(), key, value);
}

void neueda::SbfKeyValue::remove (const char* key)
{
    sbfKeyValue_remove (getHandle(), key);
}

const char* neueda::SbfKeyValue::first (void** cookie)
{
    return sbfKeyValue_first (getHandle(), cookie);
}

const char* neueda::SbfKeyValue::next (void** cookie)
{
    return sbfKeyValue_next (getHandle(), cookie);
}
