/*!
   \file sbfCommon.h
   \brief This file declares some useful structures and functions
   for general purposes.
   \Copyright 2014-2018 Neueda Ltd.
 */

#ifndef _SBF_COMMON_H_
#define _SBF_COMMON_H_

#include "sbfVersion.h"

#ifdef  __cplusplus
#define SBF_BEGIN_DECLS extern "C" {
#define SBF_END_DECLS }
#else
#define SBF_BEGIN_DECLS
#define SBF_END_DECLS
#endif

#ifdef linux
#include "sbfCommonLinux.h"
#endif
#ifdef WIN32
#include "sbfCommonWin32.h"
#endif
#ifdef __APPLE__
#include "sbfCommonDarwin.h"
#endif

#include <event.h>

#include "syshash.h"
#include "sysqueue.h"
#include "systree.h"

#include "sbfFatal.h"
#include "sbfLog.h"
#include "sbfMemory.h"

SBF_BEGIN_DECLS

/*! Minium of a and b. */
#define SBF_MIN(a, b) ((a) > (b) ? (b) : (a))
/*! Maximum of a and b. */
#define SBF_MAX(a, b) ((a) < (b) ? (b) : (a))

#define SBF_PAD(size, to) (((size) + ((to) - 1)) & ~((to) - 1))

/*! Number of items in the array. */
#define SBF_ARRAYSIZE(a) (sizeof (a) / sizeof ((a)[0]))

/*! Error type declaration */
typedef int sbfError; /* errno return */

/*! RB Tree structure definition. */
#define SBF_RB_TREE(name, suffix, entry, compare)           \
    RB_HEAD (name ## suffix ## Impl, name ## Impl);         \
    typedef struct name ## suffix ## Impl name ## suffix;   \
                                                            \
    static SBF_INLINE int                                   \
    name ## suffix ## Cmp (name lhs, name rhs)              \
    compare                                                 \
    RB_GENERATE_STATIC (name ## suffix ## Impl,             \
                        name ## Impl,                       \
                        entry,                              \
                        name ## suffix ## Cmp)

/*!
   \brief Little endian to big endian transformation.
   \param val the value to be transformed.
   \return the given value in big endian.
 */
static SBF_INLINE uint64_t
sbf_htonll (uint64_t val)
{
    uint32_t b;
    uint32_t t;

    b = htonl (val & 0xffffffff);
    t = htonl (val >> 32);

    return ((uint64_t)b << 32 | t);
}

/*! define the htonll standard as SBF implementation. */
#ifndef htonll
#define htonll sbf_htonll
#endif

/*!
   \brief From bigendian to little endian transformation.
   \param val the value to be transformed.
   \return the given value in little endian.
 */
static SBF_INLINE uint64_t
sbf_ntohll (uint64_t val)
{
    uint32_t b;
    uint32_t t;

    b = ntohl (val & 0xffffffff);
    t = ntohl (val >> 32);

    return ((uint64_t)b << 32 | t);
}

/*! define the ntohll standard as SBF implementation. */
#ifndef ntohll
#define ntohll sbf_ntohll
#endif

SBF_END_DECLS

#endif /* _SBF_COMMON_H_ */
