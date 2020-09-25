#pragma once

#include "sbfMw.h"
#include "SbfLog.hpp"
#include "SbfKeyValue.hpp"

namespace neueda
{

class SbfMw {
public:
    SbfMw (SbfLog* log, SbfKeyValue* properties) :
        mLog(log),
        mProperties(properties)
    {
        mValue = sbfMw_create (log->getHandle(), properties->getHandle());
    }

    virtual ~SbfMw ()
    {
        if (getHandle () != NULL)
            sbfMw_destroy (getHandle ());
    }

    virtual sbfMw getHandle ()
    {
        return mValue;
    }

    virtual u_int getNumThreads()
    {
        u_int ret = 0;
        if (getHandle () != NULL)
            ret = sbfMw_getNumThreads (getHandle());
        return ret;
    }
    
    virtual sbfMwThread getThread(u_int index)
    {
        sbfMwThread ret = NULL;
        if (getHandle () != NULL)
            ret = sbfMw_getThread (getHandle (), index);
        return ret;
    }

    virtual sbfMwThread getDefaultThread()
    {
        return getThread(0);
    }

    virtual SbfLog* getLog()
    {
        return mLog;
    }

    virtual SbfKeyValue* getProperties()
    {
        return mProperties;
    }

protected:
    sbfMw mValue;
    SbfLog* mLog;
    SbfKeyValue* mProperties;
};

}
