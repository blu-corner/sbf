#ifndef _SBF_GUID_H_
#define _SBF_GUID_H_

#include "sbfCommon.h"

SBF_BEGIN_DECLS

SBF_PACKED(struct sbfGuidImpl
{
    uint64_t mParts[4];
});
typedef struct sbfGuidImpl sbfGuid;

#define SBF_GUID_STRING_SIZE 68

void sbfGuid_new (sbfLog log, sbfGuid* g);
int sbfGuid_compare (const sbfGuid* g1, const sbfGuid* g2);
int sbfGuid_compareFixed (const sbfGuid* g1, const sbfGuid* g2);
void sbfGuid_copy (sbfGuid* to, const sbfGuid* from);

uint64_t sbfGuid_get (sbfGuid* g);
void sbfGuid_set (sbfGuid* g, uint64_t to);
uint64_t sbfGuid_increment (sbfGuid* g, uint64_t by);

int sbfGuid_fromString (sbfGuid* g, const char* s);
const char* sbfGuid_toString (const sbfGuid* g);
char* sbfGuid_toStringBuffer (const sbfGuid* g, char* buf, size_t len);
const char* sbfGuid_toStringFixed (const sbfGuid* g);
char* sbfGuid_toStringFixedBuffer (const sbfGuid* g, char* buf, size_t len);

SBF_END_DECLS

#endif /* _SBF_GUID_H_ */
