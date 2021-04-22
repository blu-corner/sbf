#pragma once

#include "sbfBuffer.h"

namespace neueda
{

class SbfBuffer {
public:
    /*!
        \brief Returns a memory buffer of a given size.
        \param[in] size The size of the buffer to be allocated.
        \return A SbfBuffer object.
     */
    SbfBuffer (size_t size);

    /*!
        \brief Returns a memory buffer of a given size containing the contents
               of the passed in data.
        \param[in] data The data to be copied into the buffer.
        \param[in] size The size of the buffer to be allocated.
        \return An SbfBuffer object.
     */
    SbfBuffer (void* data, size_t size);

    SbfBuffer (const SbfBuffer& other) : mBuffer {other.mBuffer}
    {
        if (mBuffer)
            sbfBuffer_addRef (mBuffer);
    }

    SbfBuffer& operator= (const SbfBuffer& other)
    {
        if (mBuffer)
            sbfBuffer_addRef (mBuffer);
        return *this;
    }

    /*!
        \brief Destructor that deletes the private buffer handler.
        \return None.
     */
    virtual ~SbfBuffer ();

    /*!
        \brief Returns a handle to the private C buffer struct.
        \return Pointer to a struct sbfBufferImpl.
     */
    virtual sbfBuffer getHandle ();

    /*!
        \brief Increases the reference count for this buffer.
        \return None.
        \see destroy
     */
    virtual void addRef ();

    /*!
        \brief Blocks the buffer to avoid content modifications. If the caller
        tries to modify the buffer after a lock operation, the program will exit.
        \return None.
     */
    virtual void lock ();

    /*!
        \brief Decreases the reference count and releases the resources allocated
        by the buffer when counts come down to zero.
        \return None.
     */
    virtual void destroy ();

    /*!
        \brief Returns the data allocated by the buffer.
        \return Void pointer to the data stored in the buffer.
     */
    virtual void* getData ();

    /*!
        \brief Links a data to the given buffer's handler (no copy).
        \param data the data to be linked to the buffer.
        \return None.
     */
    virtual void setData (void* data);

    /*!
        \brief Returns the size of the buffer.
        \return the size of the buffer.
     */
    virtual size_t getSize ();

    /*!
        \brief Sets the size of the buffer (no allocation of memory is performed).
        \param size the size of the buffer.
        \return None.
     */
    virtual void setSize (size_t size);

private:
    sbfBuffer mBuffer;

    static void (*sbfBufferDestroyCb) (sbfBuffer buffer,
                                        void* data,
                                        void* closure);
};

}
