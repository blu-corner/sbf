/*!
   \file sbfGuid.h
   \brief This file declares the structures and functions to manage unique identifiers.
   \copyright © Copyright 2016 Neueda all rights reserved.
 */

#ifndef _SBF_GUID_H_
#define _SBF_GUID_H_

#include "sbfCommon.h"

SBF_BEGIN_DECLS

/*! Private implementation of GUID */
SBF_PACKED(struct sbfGuidImpl
{
    uint64_t mParts[4];
});

/*! Declares the GUID's handler. */
typedef struct sbfGuidImpl sbfGuid;

/*! Size of GUID string */
#define SBF_GUID_STRING_SIZE 68

/*!
   \brief Returns a GUID's handler for a given
   \param log the log system's handler.
   \param g the GUID structure to be initialised.
 */
void sbfGuid_new (sbfLog log, sbfGuid* g);

/*!
   \brief Compares two GUIDs.
   \param g1 a GUID.
   \param g2 another GUID.
   \return integer telling how g1 is compared to g2.
     -1 if g1 is less than g2.
      0 if g1 is equal to g2.
      1 if g1 is greater than g2.
 */
int sbfGuid_compare (const sbfGuid* g1, const sbfGuid* g2);

/*!
   \brief Compares the fixed part of two GUIDs.
   \param g1 a GUID.
   \param g2 another GUID.
   \return integer telling how the fix parts of GUIDs are from g1 is compared to g2.
     -1 if g1 is less than g2.
      0 if g1 is equal to g2.
      1 if g1 is greater than g2.
 */
int sbfGuid_compareFixed (const sbfGuid* g1, const sbfGuid* g2);

/*!
   \brief Copies the GUID from one GUID into another one.
   \param to the target GUID.
   \param from the source GUID.
 */
void sbfGuid_copy (sbfGuid* to, const sbfGuid* from);

/*!
   \brief Returns the fixed part of the given GUID.
   \param g the full GUID.
   \return the fixed part of the given GUID.
 */
uint64_t sbfGuid_get (sbfGuid* g);

/*!
   \brief Sets the fixed part into a GUID.
   \param g the target GUID to set the fixed part.
   \param to the fixed GUID part.
 */
void sbfGuid_set (sbfGuid* g, uint64_t to);

/*!
   \brief Increments the fixed GUID part with the given number and return it.
   \param g the GUID to be incremented.
   \param by the amount to be incremented.
   \return the previous fixed GUID part incremented by the given amount.
 */
uint64_t sbfGuid_increment (sbfGuid* g, uint64_t by);

/*!
   \brief Parses the content of the GUID from a given string.
   \param g the GUID structure containing the changes after the parsing.
   \param s the string containing the GUID.
   \return 1 if successfully performed, 0 otherwise.
   \see sbfGuid_toString
 */
int sbfGuid_fromString (sbfGuid* g, const char* s);

/*!
   \brief Returns a null terminated string containing the GUID.
   \param g The GUID to be serialised into the string.
   \return a null terminated string containing the GUID.
   \see sbfGuid_fromString
 */
const char* sbfGuid_toString (const sbfGuid* g);

/*!
   \brief Serialises the GUID into a string buffer. Note that it
   uses the provided memory (does not perform a copy).
   \param g the GUID to be serialised.
   \param buf the data that will containg the GUID.
   \param len the length of the buffer.
   \return null terminated string containing the GUID (returns buf).
 */
char* sbfGuid_toStringBuffer (const sbfGuid* g, char* buf, size_t len);

/*!
   \brief Serialises the fixed part of the GUID into a null terminated string.
   \param g the GUID containing the fixed part to be serialised.
   \return the fixed part of the GUID serialised into a null terminated string.
 */
const char* sbfGuid_toStringFixed (const sbfGuid* g);

/*!
   \brief Serialises the fixed part of the GUID into a string buffer. Note that it
   uses the provided memory (does not perform a copy).
   \param g the GUID to be serialised.
   \param buf the data that will containg the fixed part of the GUID.
   \param len the length of the buffer.
   \return null terminated string containing the fixed part of the GUID (returns buf).
 */
char* sbfGuid_toStringFixedBuffer (const sbfGuid* g, char* buf, size_t len);

SBF_END_DECLS

#endif /* _SBF_GUID_H_ */
