#include "SbfPub.hpp"

neueda::SbfPub::SbfPub (neueda::SbfTport* tport,
                        neueda::SbfQueue* queue,
                        const char* topic,
                        neueda::SbfPubDelegate* delegate)
    : mDelegate (delegate)
{
    mValue = sbfPub_create (tport->getHandle (),
                            queue->getHandle (),
                            topic,
                            SbfPub::sbfPubReady,
                            this);
}

neueda::SbfPub::~SbfPub ()
{
    if (getHandle () != NULL)
        sbfPub_destroy (getHandle ());
}

sbfPub neueda::SbfPub::getHandle ()
{
    return mValue;
}

neueda::SbfBuffer* neueda::SbfPub::getBuffer (size_t size)
{
    SbfBuffer* ret = NULL;
    if (getHandle () != NULL)
    {
        sbfBuffer buf = sbfPub_getBuffer (getHandle (), size);
        ret = new neueda::SbfBuffer (buf->mData, buf->mSize);
    }
    return ret;
}

void neueda::SbfPub::sendBuffer (neueda::SbfBuffer* buffer)
{
    if (getHandle () != NULL)
    {
        sbfPub_sendBuffer (getHandle (), buffer->getHandle ());
        delete buffer;
    }
}

void neueda::SbfPub::send (void* data, size_t size)
{
    if (getHandle () != NULL)
    {
        sbfPub_send (getHandle (), data, size);
    }
}

sbfTopicImpl* neueda::SbfPub::getTopic ()
{
    sbfTopicImpl* ret = NULL;
    if (getHandle () != NULL)
    {
        ret = sbfPub_getTopic (getHandle ());
    }
    return ret;
}
