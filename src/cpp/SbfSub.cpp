#include "SbfSub.hpp"

neueda::SbfSub::SbfSub (neueda::SbfTport* tport,
                        neueda::SbfQueue* queue,
                        const char* topic,
                        neueda::SbfSubDelegate* delegate)
    : mDelegate (delegate)
{
    mValue = sbfSub_create (tport->getHandle (),
                            queue->getHandle (),
                            topic,
                            SbfSub::sbfSubReady,
                            SbfSub::sbfSubMessage,
                            this);
}

neueda::SbfSub::~SbfSub ()
{
    if (getHandle () != NULL)
        sbfSub_destroy (getHandle ());
}

sbfSub neueda::SbfSub::getHandle ()
{
    return mValue;
}

sbfTopicImpl* neueda::SbfSub::getTopic ()
{
    sbfTopicImpl* ret = NULL;
    if (getHandle () != NULL)
        ret = sbfSub_getTopic (getHandle ());
    return ret;
}
