/*!
   \file sbfMemory.h
   \brief This file declares the structures and functions to handle memory.
   \copyright © Copyright 2016 Neueda all rights reserved.
 */

#ifndef _SBF_MEMORY_H_
#define _SBF_MEMORY_H_

#include "sbfCommon.h"

SBF_BEGIN_DECLS

/*!
   \brief Allocs a block of memory of the given size.
   It will return NULL if the memory cannot be allocated.
   \param size the size of the block of memory to be allocated.
   \return a pointer to a block of memory of the given size.
 */
void* sbfMemory_malloc (size_t size);
#define xmalloc sbfMemory_malloc

/*!
   \brief Allocs a block of memory of the given size filled with zeros.
   It will return NULL if the memory cannot be allocated.
   \param nmemb the number of items to be allocated.
   \param size the size of the block of memory to be allocated.
   \return a pointer to a block of memory of the given size filled with zeros.
 */
void* sbfMemory_calloc (size_t nmemb, size_t size);
#define xcalloc sbfMemory_calloc

/*!
   \brief Reallocate memory at the given pointer with a new size.
   \param ptr the source pointer to be reallocated.
   \param nmemb the number of items to be allocated.
   \param size the size of an item.
   \return pointer to the reallocated memory.
 */
void* sbfMemory_realloc (void* ptr, size_t nmemb, size_t size);
#define xrealloc sbfMemory_realloc

/*!
   \brief Duplicate a null terminated string containing a
   duplicate of the given string.
   \param s the null terminated string to be duplicated.
   \return a null terminated string containing a duplicate of the given string.
 */
char* sbfMemory_strdup (const char* s);
#define xstrdup sbfMemory_strdup

/*!
   String formatting with variable number of arguments.
   \param ret null terminated string describing the error if any.
   \param fmt the string format.
   \param ... the variable arguments to compose the formatted string.
 */
void sbfMemory_asprintf (char** ret, const char* fmt, ...);
#define xasprintf sbfMemory_asprintf

/*!
   String formatting with variable number of arguments.
   \param ret null terminated string describing the error if any.
   \param fmt the string format.
   \param ap the variable arguments to compose the formatted string.
 */
void sbfMemory_vasprintf (char** ret, const char* fmt, va_list ap);
#define xvasprintf sbfMemory_vasprintf

SBF_END_DECLS

#endif /* _SBF_MEMORY_H_ */
