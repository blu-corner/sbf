#pragma once

#include "sbfTimer.h"
#include "SbfMw.hpp"
#include "SbfQueue.hpp"

namespace neueda
{

class SbfTimerDelegate {
public:
    virtual ~SbfTimerDelegate() { }
    virtual void onTicked () { }
};

class SbfTimer {
public:
    /*!
        \brief Constructs a subscriber object.
        \param[in] thread The thread handler.
        \param[in] queue Queue.
        \param[in] delegate A delegate object that will be called when the timer
                            fires.
        \param[in] interval Time in seconds before the timer is fires the
                            callback.
        \return A SbfSub object.
     */
    SbfTimer (sbfMwThread thread,
              neueda::SbfQueue* queue,
              neueda::SbfTimerDelegate* delegate,
              double interval)
        : mThread (thread),
          mQueue (queue),
          mDelegate (delegate),
          mInterval (interval)
    {
        mTimer = sbfTimer_create (mThread,
                                  mQueue->getHandle (),
                                  SbfTimer::sbfTimerTicked,
                                  this,
                                  mInterval);
    }

    /*!
        \brief Destructor that deletes the private timer handler.
        \return None.
     */
    virtual ~SbfTimer ()
    {
        if (getHandle () != NULL)
            sbfTimer_destroy (getHandle ());
    }

    /*!
        \brief Returns a handle to the private C timer struct.
        \return Pointer to a struct sbfTimerImpl.
     */
    virtual sbfTimer getHandle ()
    {
        return mTimer;
    }

    /*!
        \brief Reschedule the timer for the already configured interval.
        \return None.
     */
    virtual void reschedule ()
    {
        if (getHandle () != NULL)
            sbfTimer_destroy (getHandle ());


        mTimer = sbfTimer_create (mThread,
                                  mQueue->getHandle (),
                                  SbfTimer::sbfTimerTicked,
                                  this,
                                  mInterval);
    }

protected:
    sbfMwThread       mThread;
    SbfQueue*         mQueue;
    SbfTimerDelegate* mDelegate;
    double            mInterval;
    sbfTimer          mTimer;

private:
    static void sbfTimerTicked (sbfTimer timer, void* closure)
    {
        SbfTimer* instance = static_cast<SbfTimer*>(closure);

        if (instance->mDelegate)
        {
            instance->mDelegate->onTicked ();
        }
    }
};

}
