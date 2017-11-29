/*!
   \file sbfArgument.h
   \brief This file defines the helper structures and functions to handle
   typical program arguments with options.
   \copyright © Copyright 2016 Neueda all rights reserved.
 */

#ifndef _SBF_ARGUMENTS_H_
#define _SBF_ARGUMENTS_H_

#include "sbfCommon.h"

SBF_BEGIN_DECLS

/** Arguments handler */
typedef struct sbfArgumentsImpl* sbfArguments;

/*!
   Returns an argument handler to manage arguments options.
   An argc and argv is passed to getopt. Option presence is then available from
   sbfArguments_hasOption, option arguments from _optionValue and the remaining
   positional arguments from _get.
   \param options The options the caller is interested to parse.
   \param argc the number of arguments.
   \param argv array of null terminated strings contaning the arguments.
   \return an argument handler to manage arguments options.
 */
sbfArguments sbfArguments_create (const char* options, int argc, char** argv);

/*!
   Releases the arguments allocated by sbfArguments_create method.
   \param a the arguments handler.
 */
void sbfArguments_destroy (sbfArguments a);

/*!
   Returns 1 if the option is present 0 otherwise.
   \param a the argument handler.
   \param c char indicating the option (e.g. h for -h option).
   \return 1 if the option is present 0 otherwise.
 */
int sbfArguments_hasOption (sbfArguments a, u_char c);

/*!
   Returns the value associated to the option or NULL in case that
   the option does not exist.
   \param a the argument handler.
   \param c char indicating the option (e.g. h for -h option).
   \return the value associated to the option or NULL in case that
   the option does not exist.
 */
const char* sbfArguments_optionValue (sbfArguments a, u_char c);

/*!
   Returns the number of arguments held by the given argument handler.
   \param a the argument handler.
   \return the number of arguments held by the given argument handler.
 */
u_int sbfArguments_size (sbfArguments a);

/*!
   Returns a null terminated string at the given argument index. It returns
   NULL if argument is out of bounds.
   \param a the argument handler.
   \param idx the index where the argument is placed.
   \return a null terminated string at the given argument index. It returns
   NULL if argument is out of bounds.
 */
const char* sbfArguments_get (sbfArguments a, u_int idx);

/*!
   Serialise the arguments in an array of null terminated strings and
   return it into a buffer.
   \param argc the number of arguments.
   \param argv array of null terminated strings contaning the arguments.
   \param[out] buf the buffer with the serialised arguments.
   \param[out] len the lenght of the serialised buffer.
 */
void sbfArguments_pack (int argc, char** argv, char** buf, size_t* len);

/*!
   Deserialise the arguments from an array of null terminated strings and place
   them into typical argc and argv variables.
   \param buf the buffer with the serialised arguments.
   \param len the lenght of the serialised buffer.
   \param[out] argc the number of arguments.
   \param[out] argv array of null terminated strings contaning the arguments.
 */
void sbfArguments_unpack (char* buf, size_t len, int* argc, char*** argv);

/*!
   Release the memory allocated by argv.
   \param argc the number of arguments.
   \param argv array of null terminated strings contaning the arguments.
 */
void sbfArguments_free (int argc, char** argv);

SBF_END_DECLS

#endif /* _SBF_ARGUMENTS_H_ */
