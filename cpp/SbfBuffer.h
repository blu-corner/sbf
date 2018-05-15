#pragma once

#include "sbfBuffer.h"

class SbfBufferDelegate {
public:
     ~SbfBufferDelegate () { }

     void destroy (void* data, void* closure) { }
};

class SbfBuffer {
public:
    SbfBuffer  (size_t size,
                const char* function,
                u_int line,
                SbfBufferDelegate* delegate)
    {
        mBuffer = sbfBudffer_newZero(...)
    }

    virtual ~SbfBuffer ()
    {
        if (getHandle() != NULL)
            sbfBuffer_destroy (getHandle ());
    }

    virtual sbfBuffer getHandle ()
    {
        return mBuffer;
    }

    virtual void addRef ()
    {
        sbfBuffer_addRef (getHandle ());
    }

    virtual void lock ()
    {
        sbfBuffer_lock (getHandle ());
    }

    virtual void destroy ()
    {
        sbfBuffer_destroy (getHandle());
    }

    virtual void* getData ()
    {
        sbfBuffer_getData (getHandle ());
    }

    virtual void setData (void* data)
    {
        sbfBuffer_getData (getHandle (), data);
    }

    virtual size_t getSize ()
    {
        sbfBuffer_getSize (getHandle ());
    }

    virtual void setSize (size_t size)
    {
        sbfBuffer_setSize (getHandle (), size);
    }

private:
    sbfBuffer mBuffer;
    SbfBufferDelegate* mDelegate;

    static struct sbfBufferImpl* sbfBuffer;
    static void (*sbfBufferDestroyCb) (sbfBuffer buffer,
                                        void* data,
                                        void* closure);
};
