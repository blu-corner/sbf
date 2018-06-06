/*!
   \file sbfCsvParser.h
   \brief This file declares some helper functions to parse CSV files.
   \Copyright 2014-2018 Neueda Ltd.
 */
#ifndef _SBF_CSV_PARSER_H_
#define _SBF_CSV_PARSER_H_

#include "sbfCommon.h"

SBF_BEGIN_DECLS

/*!
   Given a file parses its values and returns them into fields and number of fields.
   \param f The file containint the CVS values.
   \param fields the fields containing the read values.
   \param nfields the number of fields that were read.
   \param separator the separator between fields.
   \param line the number of lines that was processed.
   \return 0 if successfully parsed, error code otherwise.
 */
sbfError sbfCsvParser_next (FILE* f,
                            char*** fields,
                            u_int* nfields,
                            u_char separator,
                            u_int* line);

/*!
   Releases the fields allocated by sbfCsvParser_next.
   \param fields the fields containing the read values.
   \param nfields the number of fields that were read.
 */
void sbfCsvParser_free (char** fields, u_int nfields);

SBF_END_DECLS

#endif
