#pragma once

#include "sbfPub.h"
#include "SbfTport.hpp"
#include "SbfQueue.hpp"
#include "SbfBuffer.hpp"

namespace neueda
{

class SbfPub;
class SbfPubDelegate {
public:
    virtual ~SbfPubDelegate() { }
    virtual void onPubReady (SbfPub* pub) { }
};

class SbfPub {
public:
    /*!
        \brief Constructs a publisher object.
        \param[in] tport Connection port.
        \param[in] queue Queue for publishing objects.
        \param[in] topic The topic.
        \param[in] delegate A delegate object for handling when the publisher
                            is ready.
        \return A SbfPub object.
     */
    SbfPub (neueda::SbfTport* tport,
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

    /*!
        \brief Destructor that deletes the private publisher handler.
        \return None.
     */
    virtual ~SbfPub ()
    {
        if (getHandle () != NULL)
            sbfPub_destroy (getHandle ());
    }

    /*!
        \brief Returns a handle to the private C publisher struct.
        \return Pointer to a struct sbfPubImpl.
     */
    virtual sbfPub getHandle ()
    {
        return mValue;
    }

    /*!
        \brief Allocates and returns a buffer.
        \param size the size of the buffer to be allocated.
        \return the buffer allocated by the publisher.
    */
    virtual neueda::SbfBuffer* getBuffer (size_t size)
    {
        SbfBuffer* ret = NULL;
        if (getHandle () != NULL)
        {
            sbfBuffer buf = sbfPub_getBuffer (getHandle (), size);
            ret = new neueda::SbfBuffer (buf->mData, buf->mSize);
        }
        return ret;
    }

    /*!
        \brief Sends a buffer throught the publisher.
        SendBuffer destroys the buffer. It must not have more than one reference.
        Messages that are too big and those sent before the ready callback are
        silently dropped.
        \param buffer the buffer to be sent.
        \return None.
    */
    virtual void sendBuffer (neueda::SbfBuffer* buffer)
    {
        if (getHandle () != NULL)
        {
            sbfPub_sendBuffer (getHandle (), buffer->getHandle ());
            delete buffer;
        }
    }

    /*!
        \brief Sends a payload through the publisher.
        \param data pointer to the payload to be sent.
        \param size the size of the payload.
        \return None.
    */
    virtual void send (void* data, size_t size)
    {
        if (getHandle () != NULL)
        {
            sbfPub_send (getHandle (), data, size);
        }
    }

    /*!
        \brief Returns the topic associated with this publisher.
        \return the topic associated to the publisher.
    */
    virtual sbfTopicImpl* getTopic ()
    {
        sbfTopicImpl* ret = NULL;
        if (getHandle () != NULL)
        {
            ret = sbfPub_getTopic (getHandle ());
        }
        return ret;
    }

protected:
    sbfPub          mValue;
    SbfPubDelegate* mDelegate;

private:
    static void sbfPubReady (sbfPub pub, void* closure)
    {
        SbfPub* instance = static_cast<SbfPub*>(closure);

        if (instance->mDelegate)
        {
            instance->mDelegate->onPubReady (instance);
        }
    }
};

}
