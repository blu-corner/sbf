#pragma once

#include "sbfTimer.h"

namespace neueda
{

class SbfTimerDelegate {
    virtual ~SbfTimerDelegate() { }
    virtual void onTicked () { }
};

class SbfTimer {
public:
    SbfTimer (sbfMwThread thread,
              sbfQueue queue,
              SbfTimerDelegate* delegate,
              double interval)
        : mThread (thread),
          mQueue (queue),
          mDelegate (delegate),
          mInterval (interval)
    {
        mTimer = sbfTimer_create (mThread,
                                  mQueue,
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

protected:
    sbfMwThread       mThread;
    sbfQueue          mQueue;
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
