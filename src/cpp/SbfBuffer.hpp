#pragma once

#include "sbfBuffer.h"

namespace neueda
{

class SbfBufferDelegate {
public:
     ~SbfBufferDelegate () { }

     void destroy (void* data, void* closure) { }
};

class SbfBuffer {
public:
    SbfBuffer (size_t size)
    {
        mBuffer = sbfBuffer_newZero (NULL, size);
    }

    SbfBuffer (void* data, size_t size)
    {
        mBuffer = sbfBuffer_copy (NULL, data, size);
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
        return sbfBuffer_getData (getHandle ());
    }

    virtual void setData (void* data)
    {
        sbfBuffer_setData (getHandle (), data);
    }

    virtual size_t getSize ()
    {
        return sbfBuffer_getSize (getHandle ());
    }

    virtual void setSize (size_t size)
    {
        sbfBuffer_setSize (getHandle (), size);
    }

private:
    sbfBuffer mBuffer;
    SbfBufferDelegate* mDelegate;

    static void (*sbfBufferDestroyCb) (sbfBuffer buffer,
                                        void* data,
                                        void* closure);
};

}
