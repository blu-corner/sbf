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

    virtual ~SbfTimer ()
    {
        if (getHandle () != NULL)
            sbfTimer_destroy (getHandle ());
    }

    virtual sbfTimer getHandle ()
    {
        return mTimer;
    }

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
