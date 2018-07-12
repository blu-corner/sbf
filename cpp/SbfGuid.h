#pragma once

#include "sbfCommon.h"

#ifndef _SBF_GUID_H_
#define _SBF_GUID_H_


#define SBF_GUID_STRING_SIZE 68

class SbfGuid {
public:
    sbfGuid (sbfLog log){
        sbfGuid_new (log, &mGuid);
    }

    void sbfGuid_new(sbfLog log, sbfGuid* g);

    virtual ~sbfGuid ()
    {
        if (getHandle() != NULL)
            sbfGuid_free (getHandle ());
    }

    virtual sbfGuid getHandle ()
    {
        return mGuid;
    }

    virtual int sbfGuid compare (const sbfGuid* g2)
    {
        return sbfGuid_compare (getHandle (), g2)
    }

    virtual int sbfGuid compareFixed (const sbfGuid* g2)
    {
        return sbfGuid_compareFixed (getHandle (), g2)
    }

    virtual uint64_t sbfGuid get (const sbfGuid* g)
    {
        return sbfGuid_get(getHandle ());
    }

    virtual sbfGuid set (const sbfGuid* g, uint64_t to)
    {
        sbfGuid_set (getHandle (), g, to);
    }

    virtual uint64_t sbfGuid increment (const sbfGuid* g, uint64_t by)
    {
        return sbfGuid_increment (getHandle (), g, by);
    }

    virtual void fromString (const char* s)
    {
        sbfGuid_fromString (getHandle (), s);
    }

    virtual std::string toString ()
    {
        return std::string(sbfGuid_toString ());
    }

private:
    sbfGuid mGuid;

};


