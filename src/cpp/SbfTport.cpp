#include "SbfTport.hpp"

neueda::SbfTport::SbfTport (SbfMw* mw,
                            const char* name,
                            uint64_t mask)
    : mMw(mw)
{
    mValue = sbfTport_create (mw->getHandle(), name, mask);
}

neueda::SbfTport::~SbfTport ()
{
    if (getHandle () != NULL)
        sbfTport_destroy (getHandle ());
}

sbfTport neueda::SbfTport::getHandle ()
{
    return mValue;
}

const char* neueda::SbfTport::getName ()
{
    const char* ret = NULL;
    if (getHandle () != NULL)
        ret = sbfTport_getName (getHandle());
    return ret;
}

neueda::SbfMw* neueda::SbfTport::getMw ()
{
    return mMw;
}
