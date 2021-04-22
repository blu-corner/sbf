#include "SbfTimer.hpp"

neueda::SbfTimer::SbfTimer (sbfMwThread thread,
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

neueda::SbfTimer::~SbfTimer ()
{
    if (getHandle () != NULL)
        sbfTimer_destroy (getHandle ());
}

sbfTimer neueda::SbfTimer::getHandle ()
{
    return mTimer;
}

void neueda::SbfTimer::reschedule ()
{
    if (getHandle () != NULL)
        sbfTimer_destroy (getHandle ());


    mTimer = sbfTimer_create (mThread,
                                mQueue->getHandle (),
                                SbfTimer::sbfTimerTicked,
                                this,
                                mInterval);
}
