#ifndef _SBF_ARGUMENTS_H_
#define _SBF_ARGUMENTS_H_

#include "sbfCommon.h"

SBF_BEGIN_DECLS

typedef struct sbfArgumentsImpl* sbfArguments;

/*
 * An argc and argv is passed to getopt. Option presence is then available from
 * sbfArguments_hasOption, option arguments from _optionValue and the remaining
 * positional arguments from _get. _toNumber may be used to convert string to a
 * number.
 */

sbfArguments sbfArguments_create (const char* options, int argc, char** argv);
void sbfArguments_destroy (sbfArguments a);

int sbfArguments_hasOption (sbfArguments a, u_char c);
const char* sbfArguments_optionValue (sbfArguments a, u_char c);

u_int sbfArguments_size (sbfArguments a);
const char* sbfArguments_get (sbfArguments a, u_int idx);

void sbfArguments_pack (int argc, char** argv, char** buf, size_t* len);
void sbfArguments_unpack (char* buf, size_t len, int* argc, char*** argv);
void sbfArguments_free (int argc, char** argv);

SBF_END_DECLS

#endif /* _SBF_ARGUMENTS_H_ */
