#include "SbfMw.hpp"

neueda::SbfMw::SbfMw (SbfLog* log, SbfKeyValue* properties) :
    mLog(log),
    mProperties(properties)
{
    mValue = sbfMw_create (log->getHandle(), properties->getHandle());
}

neueda::SbfMw::~SbfMw ()
{
    if (getHandle () != NULL)
        sbfMw_destroy (getHandle ());
}

sbfMw neueda::SbfMw::getHandle ()
{
    return mValue;
}

u_int neueda::SbfMw::getNumThreads()
{
    u_int ret = 0;
    if (getHandle () != NULL)
        ret = sbfMw_getNumThreads (getHandle());
    return ret;
}

sbfMwThread neueda::SbfMw::getThread(u_int index)
{
    sbfMwThread ret = NULL;
    if (getHandle () != NULL)
        ret = sbfMw_getThread (getHandle (), index);
    return ret;
}

sbfMwThread neueda::SbfMw::getDefaultThread()
{
    return getThread(0);
}

neueda::SbfLog* neueda::SbfMw::getLog()
{
    return mLog;
}

neueda::SbfKeyValue* neueda::SbfMw::getProperties()
{
    return mProperties;
}
