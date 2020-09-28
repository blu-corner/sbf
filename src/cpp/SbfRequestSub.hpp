#pragma once

#include "sbfRequestReply.h"
#include "SbfBuffer.hpp"
#include "SbfTport.hpp"
#include "SbfQueue.hpp"

namespace neueda
{

class SbfRequestSub;
class SbfRequestSubDelegate {
public:
    virtual ~SbfRequestSubDelegate() { }
    virtual void onSubReady (neueda::SbfRequestSub* sub) { }
    virtual void onSubRequest (neueda::SbfRequestSub* sub,
                               struct sbfRequestImpl* req,
                               neueda::SbfBuffer* buf) { }
};

class SbfRequestSub {
public:
    /*!
        \brief Constructs a replier object.
        \param[in] tport Connection port.
        \param[in] queue Queue for handling objects as they come in.
        \param[in] topic The topic.
        \param[in] delegate A delegate object for handling when the subsriber
                            is ready and receives a message.
        \return A SbfRequestSub object.
     */
    SbfRequestSub (neueda::SbfTport* tport,
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

    /*!
        \brief Destructor that deletes the private replier handler.
        \return None.
     */
    virtual ~SbfRequestSub ()
    {
        if (getHandle () != NULL)
            sbfRequestSub_destroy (getHandle ());
    }

    /*!
        \brief Returns a handle to the private C replier struct.
        \return Pointer to a struct sbfRequestSubImpl.
     */
    virtual sbfRequestSub getHandle ()
    {
        return mValue;
    }

    /*!
        \brief Sends a message with the given subscriber.
        \param req the request that is being replied.
        \param data array of bytes to be sent.
        \param size the length in bytes of the data previously defined.
        \return None.
     */
    virtual void reply (struct sbfRequestImpl* req, void* data, size_t size)
    {
        if (getHandle () != NULL)
            sbfRequestSub_reply (getHandle(), req, data, size);
    }

    /*!
        \brief Returns the topic associated with this replier.
        \return the topic associated to the replier.
    */
    virtual const char* getTopic ()
    {
        const char* ret = NULL;
        if (getHandle () != NULL)
            ret = sbfRequestSub_getTopic (getHandle ());
        return ret;
    }

protected:
    sbfRequestSub   mValue;
    neueda::SbfRequestSubDelegate* mDelegate;

private:
    static void sbfSubReady (sbfRequestSub sub, void* closure)
    {
        SbfRequestSub* instance = static_cast<SbfRequestSub*>(closure);

        if (instance->mDelegate)
        {
            instance->mDelegate->onSubReady (instance);
        }
    }

    static void sbfSubRequest (sbfRequestSub sub,
                               struct sbfRequestImpl* req,
                               sbfBuffer buffer,
                               void* closure)
    {
        SbfRequestSub* instance = static_cast<SbfRequestSub*>(closure);
        SbfBuffer buf = neueda::SbfBuffer (buffer->mData, buffer->mSize);

        if (instance->mDelegate)
        {
            instance->mDelegate->onSubRequest (instance, req, &buf);
        }
    }
};

}
