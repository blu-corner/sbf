#ifndef _SBF_INTERFACE_H_
#define _SBF_INTERFACE_H_

#include "sbfCommon.h"

SBF_BEGIN_DECLS

/*
 * Name is either an interface name (eth0) or an address in the form 10, 10.1,
 * 10.1.2, 10.1.2.3.
 */
uint32_t sbfInterface_find (const char* name);

/* Split string into host and port. */
sbfError sbfInterface_parseAddress (const char* s, struct sockaddr_in* sin);

SBF_END_DECLS

#endif /* _SBF_INTERFACE_H_ */
