/*!
   \file sbfTport.h
   \brief This file declares the structures and functions to handler port
   connections.
   \copyright © Copyright 2016 Neueda all rights reserved.
*/
#ifndef _SBF_TPORT_H_
#define _SBF_TPORT_H_

#include "sbfBatch.h"
#include "sbfCommon.h"
#include "sbfMw.h"
#include "sbfPub.h"
#include "sbfSub.h"
#include "sbfTopic.h"

SBF_BEGIN_DECLS

/** Declaration of port handler */
typedef struct sbfTportImpl* sbfTport;

/*!
   \brief Creates a port for a given port name and mask.
   \param mw middleware's handler.
   \return a handler to a connection port.
*/
sbfTport sbfTport_create (sbfMw mw, const char* name, uint64_t mask);

/*!
   \brief Releases all the resources allocated by a port.
   \param tport the port's handler.
*/
void sbfTport_destroy (sbfTport tport);

/*!
   \brief Returns the name associated to the port.
   \param tport the port's handler.
   \return the name associated to the port.
*/
const char* sbfTport_getName (sbfTport tport);

/*!
   \brief Returns the middleware associated to the port.
   \param tport the port's handler.
   \return the middleware associated to the port.
*/
sbfMw sbfTport_getMw (sbfTport tport);

SBF_END_DECLS

#endif /* _SBF_TPORT_H_ */
