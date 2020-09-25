#pragma once

#include "sbfTport.h"
#include "SbfMw.hpp"
#include <memory>

namespace neueda
{

class SbfTport {
public:
    SbfTport (SbfMw* mw, const char* name, uint64_t mask) : mMw(mw)
    {
        mValue = sbfTport_create (mw->getHandle(), name, mask);
    }

    virtual ~SbfTport ()
    {
        if (getHandle () != NULL)
            sbfTport_destroy (getHandle ());
    }

    virtual sbfTport getHandle ()
    {
        return mValue;
    }

    virtual const char* getName ()
    {
        const char* ret = NULL;
        if (getHandle () != NULL)
            ret = sbfTport_getName (getHandle());
        return ret;
    }

    virtual SbfMw* getMw ()
    {
        return mMw;
    }

protected:
    sbfTport mValue;
    SbfMw* mMw;
};

}
