#pragma once

#include "sbfTport.h"
#include "SbfMw.hpp"
#include <memory>

namespace neueda
{

class SbfTport {
public:
    /*!
        \brief Constructs a port object for the give port name and mask.
        \param[in] mw Middleware object
        \param[in] name The name for the port.
        \param[in] mask The thread mask to be used for the port connection.
        \return A SbfTport object.
     */
    SbfTport (SbfMw* mw, const char* name, uint64_t mask) : mMw(mw)
    {
        mValue = sbfTport_create (mw->getHandle(), name, mask);
    }

    /*!
        \brief Destructor that deletes the private port handler.
        \return None.
     */
    virtual ~SbfTport ()
    {
        if (getHandle () != NULL)
            sbfTport_destroy (getHandle ());
    }

    /*!
        \brief Returns a handle to the private C port struct.
        \return Pointer to a struct sbfTportImpl.
     */
    virtual sbfTport getHandle ()
    {
        return mValue;
    }

    /*!
        \brief Returns the name associated to the port.
        \return the name associated to the port.
    */
    virtual const char* getName ()
    {
        const char* ret = NULL;
        if (getHandle () != NULL)
            ret = sbfTport_getName (getHandle());
        return ret;
    }

    /*!
        \brief Returns the middleware associated to the port.
        \return the middleware associated to the port.
    */
    virtual SbfMw* getMw ()
    {
        return mMw;
    }

protected:
    sbfTport mValue;
    SbfMw* mMw;
};

}
