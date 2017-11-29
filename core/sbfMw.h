/*!
   \file sbfMw.h
   \brief This file represents the middleware API.
   \copyright © Copyright 2016 Neueda all rights reserved.
*/
#ifndef _SBF_MW_H_
#define _SBF_MW_H_

#include "sbfCommon.h"
#include "sbfEvent.h"
#include "sbfHiResTimer.h"
#include "sbfKeyValue.h"
#include "sbfQueue.h"
#include "sbfTimer.h"

SBF_BEGIN_DECLS

typedef struct sbfMwImpl* sbfMw;
typedef struct sbfMwThreadImpl* sbfMwThread;

/** Supported capabilities bitmap */
#define CAP_ALL_MASK          0x00000001 // All available capabilities
#define CAP_HI_RES_COUNTER    1 << 0     // Hi resolution counters
// TODO: Extends other capabilities here...

/** \brief Maximum number of threads */
#define SBF_MW_THREAD_LIMIT 64
/** \brief thread mask */
#define SBF_MW_ALL_THREADS (~0ULL)

//------------------------------------------------------------------------------
// Middleware API
//------------------------------------------------------------------------------

/*!
   \brief This function returns a middleware structure.
   \param log log structure used for logging purposes.
   \param properties key values defining the settings for the middleware.
   \return Pointer to middleware structure.
*/
sbfMw sbfMw_create (sbfLog log, sbfKeyValue properties);

/*!
   \brief Release the memory allocated by the middleware structure.
   \param mw pointer to middleware structure.
   \see sbfMw_create.
*/
void sbfMw_destroy (sbfMw mw);

/*!
   \brief Returns the number of thread defined at the middleware.
   \param mw pointer to middleware structure.
   \return  the number of thread defined at the middleware.
*/
u_int sbfMw_getNumThreads (sbfMw mw);
/*!
   \brief Returns the thread for the given index
    in the range [0, sbfMw_getNumThreads].
   \param mw pointer to middleware structure.
   \param index the index of the thread in the thread pool.
   \return  the number of thread defined at the middleware.
*/
sbfMwThread sbfMw_getThread (sbfMw mw, u_int index);

/** Returns the thread by default */
#define sbfMw_getDefaultThread(mw) sbfMw_getThread (mw, 0)

/*!
   \brief Returns the log associated to the middleware.
   \param mw the middleware.
   \return the log associated to the middleware.
*/
sbfLog sbfMw_getLog (sbfMw mw);

/*!
   \brief Returns the properties settings for the middleware.
   \param mw the middleware.
   \return the properties settings for the middleware.
*/
sbfKeyValue sbfMw_getProperties (sbfMw mw);

//------------------------------------------------------------------------------
// Thread helper functions
//------------------------------------------------------------------------------

/*!
   \brief Returns the index of a thread handler.
   \param thread the thread handler being querried.
   \return the index of a thread handler.
*/
u_int sbfMw_getThreadIndex (sbfMwThread thread);

/*!
   \brief Returns the mask for a given thread handler.
   \return the mask for a given thread handler.
   \param thread the thread handler.
*/
uint64_t sbfMw_getThreadMask (sbfMwThread thread);

/*!
   \brief Returns the event base for a given thread.
   \param thread the thread handler.
   \return the event base for a given thread.
*/
struct event_base* sbfMw_getThreadEventBase (sbfMwThread thread);

/*!
 \brief Returns CAP_SUPPORTED if feature is supported,
 otherwise CAP_UNSUPPORTED is returned.
 \param feature_mask the mask indicating the capabilities to be check.
 \return capabilities supported.
 */
uint32_t sbfMw_checkSupported(uint32_t cap_mask);

SBF_END_DECLS

#endif /* _SBF_MW_H_ */
