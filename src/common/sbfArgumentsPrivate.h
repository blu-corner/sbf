#ifndef _SBF_ARGUMENTS_PRIVATE_H_
#define _SBF_ARGUMENTS_PRIVATE_H_

SBF_BEGIN_DECLS

struct sbfArgumentsImpl
{
    char         mOptions[UCHAR_MAX];
    const char*  mValues[UCHAR_MAX];

    const char** mArguments;
    u_int        mArgumentsSize;
};

SBF_END_DECLS

#endif /* _SBF_ARGUMENTS_PRIVATE_H_ */
