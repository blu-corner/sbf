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
    SbfKeyValue ();

    /*!
        \brief Destructor that deletes the private key table handler.
        \return None.
     */
    virtual ~SbfKeyValue ();

    /*!
        \brief Returns a handle to the private C key value table struct.
        \return Pointer to a struct sbfKeyValueImpl.
     */
    virtual sbfKeyValue getHandle ();

    /*!
        \brief Returns a copy of the key value table struct.
        \return Pointer to a copy of the struct sbfKeyValueImpl.
     */
    virtual sbfKeyValue copy ();

    /*!
        \brief Returns the size of the key value table.
        \return The size of the key value table.
     */
    virtual unsigned int size ();

    /*!
        Returns the value from a given key. Returns a null terminating string
        containing the value or null if the key was not found.
        \param key null terminating string containing the key.
        \return the value from a given key. Returns a null terminating string
        containing the value or null if the key was not found.
    */
    virtual const char* get (const char* key);

    /*!
        Returns the value from a given key in an sprintf format like.
        Returns a null terminating string containing the value
        or null if the key was not found.
        \param fmt The string containing the format of the key
        \param ... the list of parameters that will compose the key.
        \return the value from a given key. Returns a null terminating string
        containing the value or null if the key was not found.
    */
    virtual const char* getV (const char* fmt, ...);

    /*!
        Adds an key and value to the key value table.
        \param key null terminating string containing the key.
        \param value the value to be associated to the given key.
        \return None.
    */
    virtual void put (const char* key, const char* value);

    /*!
        Removes a key value from the given key value table.
        Note that the calling on this method will invalidate first/next.
        \param table the key value table's handler.
        \param key identifying the item to be removed.
    */
    virtual void remove (const char* key);

    /*!
        This method will return the first element, and can be used together with
        next to iterate over the elements stored in the key value table.
        \param cookie the key value item placed at the first position.
        \return the value returned at the first position.
        \see next
    */
    virtual const char* first (void** cookie);

    /*!
        This method will return the next element stored in the key value table.
        \param cookie the key value item placed at the next position.
        \return the value returned at the next position.
        \see first
    */
    virtual const char* next (void** cookie);

protected:
    sbfKeyValue mValue;
};

}
