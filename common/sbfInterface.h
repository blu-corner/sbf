/*!
   \file sbfInterface.h
   \brief This file declares the structures and functions to manage
   network interfaces.
   \Copyright 2014-2018 Neueda Ltd.
 */

#ifndef _SBF_INTERFACE_H_
#define _SBF_INTERFACE_H_

#include "sbfCommon.h"

SBF_BEGIN_DECLS

/*!
   Get first interface that isn't local.
   \param log
   \return
 */
uint32_t sbfInterface_first (sbfLog log);

/*!
   \brief Name is either an interface name (eth0) or an address in the form 10, 10.1,
   10.1.2, 10.1.2.3.
   \param log the log system's handler.
   \param name the name of the interface to be found.
   \return the address of the interface or 0 if not found.
 */
uint32_t sbfInterface_find (sbfLog log, const char* name);

/*!
   \brief Find one interface from comma-separated list.
   \param log the log system's handler.
   \param names the list of names provided for the search.
   \param name the name of the interface found or NULL if not found.
   \return the address of the interface or 0 if not found.
 */
uint32_t sbfInterface_findOne (sbfLog log,
                               const char* names,
                               const char** name);

/*!
   \brief Split string of form 1.2.3.4:1234 into host and port.
   \param s Null terminated string containing the host and port to be parsed.
   \param sin the structure to be filled with the host and port.
   \return 0 if successful, error code otherwise.
 */
sbfError sbfInterface_parseAddress (const char* s, struct sockaddr_in* sin);

/*!
   \brief Split string into host and port and resolve host.
   \param s Null terminated string containing the host and port to be parsed.
   \param sin the structure to be filled with the host and port.
   \param error null terminated string containing a description of the error
   if any.
   \return 0 if successful, error code otherwise.
 */
sbfError sbfInterface_resolveAddress (const char* s,
                                      struct sockaddr_in* sin,
                                      const char** error);

SBF_END_DECLS

#endif /* _SBF_INTERFACE_H_ */
