#pragma once

#include "sbfSub.h"
#include "SbfTport.hpp"
#include "SbfQueue.hpp"
#include "SbfBuffer.hpp"

namespace neueda
{

class SbfSub;
class SbfSubDelegate {
public:
    virtual ~SbfSubDelegate() { }
    virtual void onSubReady (neueda::SbfSub* sub) { }
    virtual void onSubMessage (neueda::SbfSub* sub,
                               neueda::SbfBuffer* buffer) { }
};

class SbfSub {
public:
    /*!
        \brief Constructs a subscriber object.
        \param[in] tport Connection port.
        \param[in] queue Queue for handling objects as they come in.
        \param[in] topic The topic.
        \param[in] delegate A delegate object for handling when the subsriber
                            is ready and receives a message.
        \return A SbfSub object.
     */
    SbfSub (neueda::SbfTport* tport,
            neueda::SbfQueue* queue,
            const char* topic,
            neueda::SbfSubDelegate* delegate);

    /*!
        \brief Destructor that deletes the private subscriber handler.
        \return None.
     */
    virtual ~SbfSub ();

    /*!
        \brief Returns a handle to the private C subscriber struct.
        \return Pointer to a struct sbfSubImpl.
     */
    virtual sbfSub getHandle ();

    /*!
        \brief Returns the topic associated with this subscriber.
        \return the topic associated to the subscriber.
    */
    virtual sbfTopicImpl* getTopic ();

protected:
    sbfSub          mValue;
    neueda::SbfSubDelegate* mDelegate;

private:
    static void sbfSubReady (sbfSub sub, void* closure)
    {
        SbfSub* instance = static_cast<SbfSub*>(closure);

        if (instance->mDelegate)
        {
            instance->mDelegate->onSubReady (instance);
        }
    }

    static void sbfSubMessage (sbfSub sub, sbfBuffer buffer, void* closure)
    {
        SbfSub* instance = static_cast<SbfSub*>(closure);
        SbfBuffer buf = neueda::SbfBuffer (buffer->mData, buffer->mSize);

        if (instance->mDelegate)
        {
            instance->mDelegate->onSubMessage (instance, &buf);
        }
    }
};

}
