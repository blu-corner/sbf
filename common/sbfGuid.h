#ifndef _SBF_GUID_H_
#define _SBF_GUID_H_

#include "sbfCommon.h"

SBF_BEGIN_DECLS

typedef SBF_PACKED(struct
{
    uint64_t mParts[4];
}) sbfGuid;

#define SBF_GUID_STRING_SIZE 64

void sbfGuid_new (sbfGuid* g, uint64_t seed);
int sbfGuid_compare (const sbfGuid* g1, const sbfGuid* g2);
sbfGuid* sbfGuid_increment (sbfGuid* g, u_int n);
char* sbfGuid_toString (const sbfGuid* g, char* buf, size_t len);

SBF_END_DECLS

#endif /* _SBF_GUID_H_ */
