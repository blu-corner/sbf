#pragma once

#include "sbfMw.h"
#include "SbfLog.hpp"
#include "SbfKeyValue.hpp"

namespace neueda
{

class SbfMw {
public:
    /*!
        \brief Constructs a middleware object
        \param[in] log Pointer to a SbfLog object used for logging purposes.
        \param[in] properties Pointer to a SbfKeyValue object that defines the
                              settings for the middleware.
        \return A SbfMw object.
     */
    SbfMw (SbfLog* log, SbfKeyValue* properties);

    /*!
        \brief Destructor that deletes the private middleware handler.
        \return None.
     */
    virtual ~SbfMw ();

    /*!
        \brief Returns a handle to the private C middleware struct.
        \return Pointer to a struct sbfMwImpl.
     */
    virtual sbfMw getHandle ();

    /*!
        \brief Returns the number of threads defined at the middleware.
        \return  the number of thread defined at the middleware.
    */
    virtual u_int getNumThreads();

    /*!
        \brief Returns the thread for the given index
               in the range [0, getNumThreads].
        \param[in] index the index of the thread in the thread pool.
        \return sbfMwThread at the given index.
    */
    virtual sbfMwThread getThread(u_int index);

    /*!
        \brief Returns the default thread.
        \return sbfMwThread of the default thread.
    */
    virtual sbfMwThread getDefaultThread();

    /*!
        \brief Returns the logger linked to the middleware
        \return SbfLog pointer
    */
    virtual SbfLog* getLog();

    /*!
        \brief Returns the key value table object that contains the config
               for the current middleware.
        \return SbfKeyValue pointer
    */
    virtual SbfKeyValue* getProperties();

protected:
    sbfMw mValue;
    SbfLog* mLog;
    SbfKeyValue* mProperties;
};

}
