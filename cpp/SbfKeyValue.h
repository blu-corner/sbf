#pragma once

#include "sbfKeyValue.h"

class SbfKeyValue {
public:
    SbfKeyValue (void);

    virtual ~SbfKeyValue ();

    virtual const copy ();

    virtual unsigned int size ();

    virtual const char* get (const char* key);

    virtual const char* getV (const char* fmt, ...) SBF_PRINTFLIKE(2, 3);

    virtual void put (const char* key, const char* value);

    virtual void remove (const char* key);

    virtual const char* first (void** cookie);

    virtual const char* next (void** cookie);

private:
    sbfKeyValue mValue;
};
