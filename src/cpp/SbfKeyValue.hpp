#pragma once

#include "sbfKeyValue.h"

namespace neueda
{

class SbfKeyValue {
public:
    /*!
        \brief Constructs a key value table and stores the handler in the object.
        \param[in] size The size of the buffer to be allocated.
        \return A SbfBuffer object.
     */
    SbfKeyValue ()
    {
        mValue = sbfKeyValue_create ();
    }

    /*!
        \brief Destructor that deletes the private key table handler.
        \return None.
     */
    virtual ~SbfKeyValue ()
    {
        if (getHandle () != NULL)
            sbfKeyValue_destroy (getHandle ());
    }

    /*!
        \brief Returns a handle to the private C key value table struct.
        \return Pointer to a struct sbfKeyValueImpl.
     */
    virtual sbfKeyValue getHandle ()
    {
        return mValue;
    }

    /*!
        \brief Returns a copy of the key value table struct.
        \return Pointer to a copy of the struct sbfKeyValueImpl.
     */
    virtual sbfKeyValue copy ()
    {
        return sbfKeyValue_copy (getHandle ());
    }

    /*!
        \brief Returns the size of the key value table.
        \return The size of the key value table.
     */
    virtual unsigned int size ()
    {
        return sbfKeyValue_size (getHandle ());
    }

    /*!
        Returns the value from a given key. Returns a null terminating string
        containing the value or null if the key was not found.
        \param key null terminating string containing the key.
        \return the value from a given key. Returns a null terminating string
        containing the value or null if the key was not found.
    */
    virtual const char* get (const char* key)
    {
        return sbfKeyValue_get (getHandle (), key);
    }

    /*!
        Returns the value from a given key in an sprintf format like.
        Returns a null terminating string containing the value
        or null if the key was not found.
        \param fmt The string containing the format of the key
        \param ... the list of parameters that will compose the key.
        \return the value from a given key. Returns a null terminating string
        containing the value or null if the key was not found.
    */
    virtual const char* getV (const char* fmt, ...)
    {
        va_list     ap;
        char*       key;
        const char* value;

        va_start (ap, fmt);
        xvasprintf (&key, fmt, ap);
        va_end (ap);

        value = get (key);
        free (key);
        return value;
    }

    /*!
        Adds an key and value to the key value table.
        \param key null terminating string containing the key.
        \param value the value to be associated to the given key.
        \return None.
    */
    virtual void put (const char* key, const char* value)
    {
        sbfKeyValue_put (getHandle(), key, value);
    }

    /*!
        Removes a key value from the given key value table.
        Note that the calling on this method will invalidate first/next.
        \param table the key value table's handler.
        \param key identifying the item to be removed.
    */
    virtual void remove (const char* key)
    {
        sbfKeyValue_remove (getHandle(), key);
    }

    /*!
        This method will return the first element, and can be used together with
        next to iterate over the elements stored in the key value table.
        \param cookie the key value item placed at the first position.
        \return the value returned at the first position.
        \see next
    */
    virtual const char* first (void** cookie)
    {
        return sbfKeyValue_first (getHandle(), cookie);
    }

    /*!
        This method will return the next element stored in the key value table.
        \param cookie the key value item placed at the next position.
        \return the value returned at the next position.
        \see first
    */
    virtual const char* next (void** cookie)
    {
        return sbfKeyValue_next (getHandle(), cookie);
    }

protected:
    sbfKeyValue mValue;
};

}
