/*!
   \file sbfFatal.h
   \brief This file declares the structures and functions to handle critical errors.
   \Copyright 2014-2018 Neueda Ltd.
 */
#ifndef _SBF_FATAL_H_
#define _SBF_FATAL_H_

#include "sbfCommon.h"

SBF_BEGIN_DECLS

/*!
   \brief Terminate program with a given message.
   \param msg the message explaining the cause of termination.
 */
SBF_DEAD void sbfFatal_die (const char* msg);

/** Declares macro to terminate program with a given message. */
#define SBF_FATAL(msg) sbfFatal_die (msg)

SBF_END_DECLS

#endif /* _SBF_FATAL_H_ */
