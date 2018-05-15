#pragma once

#include "sbfCommon.h"

#ifndef _SBF_GUID_H_
#define _SBF_GUID_H_


#define SBF_GUID_STRING_SIZE 68

class SbfGuidImpl {
public:
    SBF_PACKED(SbfGuidImpl
{
    uint64_t mParts[4];
)};

class SbfGuidDelegate {
public:
    ~SbfGuidDelegate () { }

    void destroy (...) { }
};

class SbfGuid {
public:
    SbfGuid () { }

    {
        mGuid = sbfGuid_new(...)
    }

    virtual ~sbfGuid ()
    {
        if (getHandle() != NULL)
            sbfGuid_destroy (getHandle ());
    }

    virtual sbfGuid getHandle ()
    {
        return mGuid;
    }

    virtual sbfGuid compare (const sbfGuid* gl, const sbfGuid* g2)
    {
        sbfGuid_compare (getHandle (), g1, g2)
    }

    virtual sbfGuid compareFixed (const sbfGuid* g1, const sbfGuid* g2)
    {
        sbfGuid_compareFixed (getHandle (), g1, g2)
    }

    virtual sbfGuid get (const sbfGuid* g)
    {
        sbfGuid_get (getHandle (), g);
    }

    virtual sbfGuid set (const sbfGuid* g, uint64_t by)
    {
        sbfGuid_set (getHandle (), g, by);
    }

    virtual sbfGuid increment (const sbfGuid* g, uint64_t by)
    {
        sbfGuid_increment (getHandle (), g, by);
    }

    virtual sbfGuid fromString (const sbfGuid* g, const char* s)
    {
        sbfGuid_fromString (getHandle (), g, s);
    }

    virtual sbfGuid toString (const sbfGuid* g)
    {
        sbfGuid_toString (getHandle (), g);
    }

    virtual sbfGuid toStringBuffer (const sbfGuid* g, char* buf, size_t len)
    {
        sbfGuid_toStringBuffer (getHandle (), g, buf, len);
    }

    virtual sbfGuid toStringFixed (const sbfGuid* g)
    {
        sbfGuid_toStringFixed (getHandle (), g);
    }

    virtual sbfGuid toStringFixedBuffer (const sbfGuid* g, char* buf, size_t len)
    {
        sbfGuid_toStringFixedBuffer (getHandle (), g, buf, len);
    }

private:
    sbfGuid mGuid;
    SbfGuidDelegate* mGuid;

    static struct sbfGuidImpl* sbfGuid;
};


