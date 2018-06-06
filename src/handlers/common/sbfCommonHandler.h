/*!
   \file sbfCommonHandler.h
   \brief This file declares the structures and functions for a common
   message handler.
   \Copyright 2014-2018 Neueda Ltd.
*/
#ifndef _SBF_COMMON_HANDLER_H_
#define _SBF_COMMON_HANDLER_H_

#include "sbfInterface.h"
#include "sbfMw.h"
#include "sbfTport.h"

SBF_BEGIN_DECLS

/** \brief Multi cast topic resolver. */
typedef struct sbfMcastTopicResolverImpl* sbfMcastTopicResolver;

/*!
   \brief Create a multicast topic resolver.
   \param address the address to resolve
   \param log the log handler.
   \return a multicast topic resolver.
*/
sbfMcastTopicResolver sbfMcastTopicResolver_create (const char* address,
                                                    sbfLog log);

/*!
   \brief Release the resources allocated by the multicast topic resolver.
   \param mtr the resolver handler.
*/
void sbfMcastTopicResolver_destroy (sbfMcastTopicResolver mtr);

/*!
   \brief creates an address for a given resolver and topic.
   \param mtr the resolver handler.
   \param topic the topic / logical channel .
   \return "Return of the function"
*/
uint32_t sbfMcastTopicResolver_makeAddress (sbfMcastTopicResolver mtr,
                                            sbfTopic topic);

SBF_END_DECLS

#endif /* _SBF_COMMON_HANDLER_H_ */
