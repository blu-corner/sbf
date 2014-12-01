#ifndef _SBF_INTERFACE_H_
#define _SBF_INTERFACE_H_

#include "sbfCommon.h"

SBF_BEGIN_DECLS

/* Get first interface that isn't local. */
uint32_t sbfInterface_first (sbfLog log);

/*
 * Name is either an interface name (eth0) or an address in the form 10, 10.1,
 * 10.1.2, 10.1.2.3.
 */
uint32_t sbfInterface_find (sbfLog log, const char* name);

/* Find one interface from comma-separated list. */
uint32_t sbfInterface_findOne (sbfLog log,
                               const char* names,
                               const char** name);

/* Split string of form 1.2.3.4:1234 into host and port. */
sbfError sbfInterface_parseAddress (const char* s, struct sockaddr_in* sin);

/* Split string into host and port and resolve host. */
sbfError sbfInterface_resolveAddress (const char* s,
                                      struct sockaddr_in* sin,
                                      const char** error);

SBF_END_DECLS

#endif /* _SBF_INTERFACE_H_ */
