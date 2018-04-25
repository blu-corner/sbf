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
                SbfBufferDelegate* delegate);

    virtual ~SbfBuffer ();

    virtual SbfBuffer getNew (size_t size);

    virtual SbfBuffer newZero (size_t size);

    virtual SbfBuffer copy (void* data, size_t size);

    virtual SbfBuffer wrap (void* data, size_t size);

    virtual void addRef (sbfBuffer buffer);

    virtual void lock (sbfBuffer buffer);

    virtual void destroy (sbfBuffer buffer);

    virtual void* getData (sbfBuffer buffer);

    virtual void setData (sbfBuffer buffer, void* data);

    virtual size_t getSize (sbfBuffer buffer);

    virtual void setSize (sbfBuffer buffer, size_t size);

private:
    SbfBuffer mBuffer;
    SbfBufferDelegate* mDelegate;

    static struct sbfBufferImpl* sbfBuffer;
    static void (*sbfBufferDestroyCb) (sbfBuffer buffer,
                                        void* data,
                                        void* closure);
};
