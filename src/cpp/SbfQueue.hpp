#pragma once

#include "sbfQueue.h"
#include "SbfMw.hpp"

namespace neueda
{

class SbfQueueDelegate
{
public:
    virtual ~SbfQueueDelegate() { }

    virtual void onQueueItem () { }
};

class SbfQueue
{
public:
    /*!
        \brief Returns a queue object for the given name.
        \param[in] mw Middleware handler.
        \param[in] name The name of the topic assigned to the queue.
        \return A queue object for the given name.
    */
    SbfQueue (SbfMw* mw, const char* name);

    /*!
        \brief Destructor releases the memory allocated to the given queue.
    */
    virtual ~SbfQueue ();

    /*!
        \brief Destroy the queue which will causes the dispatch thread to end.
    */
    virtual void destroy();

    /*!
        \brief Returns a handle to the private C queue struct.
        \return Pointer to a struct sbfQueueImpl.
     */
    virtual sbfQueue getHandle ();

    /*!
        \brief Returns the name of the topic assigned to the queue.
        \return null terminated string containing the name of the queue (topic).
    */
    const char* getName ();

    /*!
        \brief Adds an item to the queue.
        \param[in] Object derived from SbfQueueDelegate that will be called
                   when popped from the queue.
        \return None.
    */
    void enqueue (SbfQueueDelegate* item);

    /*!
        \brief Dispatch all of the items that belongs to the given queue.
        \return None.
    */
    void dispatch ();

private:
    static void sbfQueueCb (sbfQueueItem queueItem, void* closure)
    {
        SbfQueueDelegate* instance = static_cast<SbfQueueDelegate*>(closure);

        if (instance)
        {
            instance->onQueueItem ();
        }
    }

    sbfQueue mQueue;
};

}
