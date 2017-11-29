/*!
   \file sbfNumberString.h
   \brief Helper file to convert from string to number.
   \copyright © Copyright 2016 Neueda all rights reserved.
 */

#ifndef _SBF_NUMBER_STRING_H_
#define _SBF_NUMBER_STRING_H_

#include "sbfCommon.h"

SBF_BEGIN_DECLS

/*!
   \brief Converts from string to int 64. Performs a boundary check
   and if number is out of range 0 is returned and error string is
   indicating the problem. If no errors then null string is returned.
   \param s The string containing the number to be converted.
   \param min the minimum number defining the beginning of the range.
   \param max the minimum number defining the ending of the range.
   \param error the error message that is returned in case that
   conversion went wrong.
   \return integer value from string to number conversion.
 */
int64_t sbfNumberString_toInt64 (const char* s,
                                 int64_t min,
                                 int64_t max,
                                 const char** error);

/*!
 *
   \param s null terminated string containing a double precision number.
   \param error the error message that is returned in case that
   conversion went wrong.
   \return double value from string to number conversion.
 */
double sbfNumberString_toDouble (const char* s, const char** error);

SBF_END_DECLS

#endif /* _SBF_NUMBER_STRING_H_ */
