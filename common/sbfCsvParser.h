#ifndef _SBF_CSV_PARSER_H_
#define _SBF_CSV_PARSER_H_

#include "sbfCommon.h"

SBF_BEGIN_DECLS

sbfError sbfCsvParser_next (FILE* f, char*** fields, u_int* nfields);
void sbfCsvParser_free (char** fields, u_int nfields);

SBF_END_DECLS

#endif
