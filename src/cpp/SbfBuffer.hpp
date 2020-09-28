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
    /*!
        \brief Returns a memory buffer of a given size.
        \param[in] size The size of the buffer to be allocated.
        \return A SbfBuffer object.
     */
    SbfBuffer (size_t size)
    {
        mBuffer = sbfBuffer_newZero (NULL, size);
    }

    /*!
        \brief Returns a memory buffer of a given size containing the contents
               of the passed in data.
        \param[in] data The data to be copied into the buffer.
        \param[in] size The size of the buffer to be allocated.
        \return An SbfBuffer object.
     */
    SbfBuffer (void* data, size_t size)
    {
        mBuffer = sbfBuffer_copy (NULL, data, size);
    }

    /*!
        \brief Destructor that deletes the private buffer handler.
        \return None.
     */
    virtual ~SbfBuffer ()
    {
        if (getHandle() != NULL)
            sbfBuffer_destroy (getHandle ());
    }

    /*!
        \brief Returns a handle to the private C buffer struct.
        \return Pointer to a struct sbfBufferImpl.
     */
    virtual sbfBuffer getHandle ()
    {
        return mBuffer;
    }

    /*!
        \brief Increases the reference count for this buffer.
        \return None.
        \see destroy
     */
    virtual void addRef ()
    {
        sbfBuffer_addRef (getHandle ());
    }

    /*!
        \brief Blocks the buffer to avoid content modifications. If the caller
        tries to modify the buffer after a lock operation, the program will exit.
        \return None.
     */
    virtual void lock ()
    {
        sbfBuffer_lock (getHandle ());
    }

    /*!
        \brief Decreases the reference count and releases the resources allocated
        by the buffer when counts come down to zero.
        \return None.
     */
    virtual void destroy ()
    {
        sbfBuffer_destroy (getHandle());
    }

    /*!
        \brief Returns the data allocated by the buffer.
        \return Void pointer to the data stored in the buffer.
     */
    virtual void* getData ()
    {
        return sbfBuffer_getData (getHandle ());
    }

    /*!
        \brief Links a data to the given buffer's handler (no copy).
        \param data the data to be linked to the buffer.
        \return None.
     */
    virtual void setData (void* data)
    {
        sbfBuffer_setData (getHandle (), data);
    }

    /*!
        \brief Returns the size of the buffer.
        \return the size of the buffer.
     */
    virtual size_t getSize ()
    {
        return sbfBuffer_getSize (getHandle ());
    }

    /*!
        \brief Sets the size of the buffer (no allocation of memory is performed).
        \param size the size of the buffer.
        \return None.
     */
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
