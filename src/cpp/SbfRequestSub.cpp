#include "SbfRequestSub.hpp"

neueda::SbfRequestSub::SbfRequestSub (neueda::SbfTport* tport,
                                      neueda::SbfQueue* queue,
                                      const char* topic,
                                      neueda::SbfRequestSubDelegate* delegate)
    : mDelegate (delegate)
{
    mValue = sbfRequestSub_create (tport->getHandle (),
                                    queue->getHandle (),
                                    topic,
                                    SbfRequestSub::sbfSubReady,
                                    SbfRequestSub::sbfSubRequest,
                                    this);
}

neueda::SbfRequestSub::~SbfRequestSub ()
{
    if (getHandle () != NULL)
        sbfRequestSub_destroy (getHandle ());
}

sbfRequestSub neueda::SbfRequestSub::getHandle ()
{
    return mValue;
}

void neueda::SbfRequestSub::reply (struct sbfRequestImpl* req, void* data, size_t size)
{
    if (getHandle () != NULL)
        sbfRequestSub_reply (getHandle(), req, data, size);
}

const char* neueda::SbfRequestSub::getTopic ()
{
    const char* ret = NULL;
    if (getHandle () != NULL)
        ret = sbfRequestSub_getTopic (getHandle ());
    return ret;
}
