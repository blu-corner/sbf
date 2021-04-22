#include "SbfRequestPub.hpp"

neueda::SbfRequestPub::SbfRequestPub (neueda::SbfTport* tport,
                                      neueda::SbfQueue* queue,
                                      const char* topic,
                                      neueda::SbfRequestPubDelegate* delegate)
    : mDelegate (delegate)
{
    mValue = sbfRequestPub_create (tport->getHandle (),
                                    queue->getHandle (),
                                    topic,
                                    SbfRequestPub::sbfPubReady,
                                    this);
}

neueda::SbfRequestPub::~SbfRequestPub ()
{
    if (getHandle () != NULL)
        sbfRequestPub_destroy (getHandle ());
}

sbfRequestPub neueda::SbfRequestPub::getHandle ()
{
    return mValue;
}

void neueda::SbfRequestPub::send (void* data, size_t size)
{
    if (getHandle () != NULL)
        sbfRequestPub_send (getHandle (),
                            data,
                            size,
                            SbfRequestPub::sbfPubReply,
                            this);
}

const char* neueda::SbfRequestPub::getTopic ()
{
    const char* ret = NULL;
    if (getHandle () != NULL)
        ret = sbfRequestPub_getTopic (getHandle ());
    return ret;
}
