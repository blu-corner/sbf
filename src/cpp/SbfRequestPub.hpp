#pragma once

#include "sbfRequestReply.h"
#include "SbfTport.hpp"
#include "SbfQueue.hpp"
#include "SbfBuffer.hpp"

namespace neueda
{

class SbfRequestPub;
class SbfRequestPubDelegate {
public:
    virtual ~SbfRequestPubDelegate() { }
    virtual void onPubReady (neueda::SbfRequestPub* pub) { }
    virtual void onPubReply (neueda::SbfRequestPub* pub,
                             struct sbfRequestImpl* req,
                             neueda::SbfBuffer* buf) { }
};

class SbfRequestPub {
public:
    /*!
        \brief Constructs a requester object.
        \param[in] tport Connection port.
        \param[in] queue Queue for publishing objects.
        \param[in] topic The topic.
        \param[in] delegate A delegate object for handling when the publisher
                            is ready.
        \return A SbfRequestPub object.
     */
    SbfRequestPub (neueda::SbfTport* tport,
                   neueda::SbfQueue* queue,
                   const char* topic,
                   neueda::SbfRequestPubDelegate* delegate);

    /*!
        \brief Destructor that deletes the private requester handler.
        \return None.
     */
    virtual ~SbfRequestPub ();

    /*!
        \brief Returns a handle to the private C requester struct.
        \return Pointer to a struct sbfRequestPubImpl.
     */
    virtual sbfRequestPub getHandle ();

    /*!
        \brief Sends a payload as a request.
        \param[in] data pointer to the payload to be sent.
        \param[in] size the size of the payload.
        \return None.
    */
    virtual void send (void* data, size_t size);

    /*!
        \brief Returns the topic associated with this requester.
        \return the topic associated to the requester.
    */
    virtual const char* getTopic ();

protected:
    sbfRequestPub   mValue;
    neueda::SbfRequestPubDelegate* mDelegate;

private:
    static void sbfPubReady (sbfRequestPub pub, void* closure)
    {
        SbfRequestPub* instance = static_cast<SbfRequestPub*>(closure);

        if (instance->mDelegate)
        {
            instance->mDelegate->onPubReady (instance);
        }
    }

    static void sbfPubReply (sbfRequestPub pub,
                             struct sbfRequestImpl* req,
                             sbfBuffer buffer,
                             void* closure)
    {
        SbfRequestPub* instance = static_cast<SbfRequestPub*>(closure);
        SbfBuffer buf = neueda::SbfBuffer (buffer->mData, buffer->mSize);

        if (instance->mDelegate)
        {
            instance->mDelegate->onPubReply (instance, req, &buf);
        }
    }
};

}
