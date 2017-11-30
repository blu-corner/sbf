/*!
   \file sbfTopic.h
   \brief Declaration of functions and structures to handle topics.
   \copyright © Copyright 2016 Neueda all rights reserved.
*/
#ifndef _SBF_TOPIC_H_
#define _SBF_TOPIC_H_

#include "sbfCommon.h"

SBF_BEGIN_DECLS

/** Public declaration for topic type */
typedef struct sbfTopicImpl* sbfTopic;

/** Maximum size of topic name */
#define SBF_TOPIC_SIZE_LIMIT 255

/** Types of topics */
typedef enum
{
    SBF_TOPIC_PUB, // publisher
    SBF_TOPIC_SUB  // subscriber
} sbfTopicType;

/*!
   \brief Returns a topic handler for a given type and name.
    A topic name is composed of three sections split by a slash
    (e.g. neueda/test/mytopic).
   \param type the type of topic (SBF_TOPIC_PUB or SBF_TOPIC_SUB).
   \param string the name for the topic to be created.
   \return a topic handler for a given type and name.
   \see sbfTopic_getTopic sbfTopic_getFirst sbfTopic_getSecond sbfTopic_getThird
*/
sbfTopic sbfTopic_create (sbfTopicType type, const char* string);

/*!
   \brief Releases all the resources allocated by the given topic handler.
   \param topic topic handler.
*/
void sbfTopic_destroy (sbfTopic topic);

/*!
   \brief Create another topic and returns its handler
   \param topic the topic handler to be copied.
   \param topic create another topic and returns its handler.
*/
sbfTopic sbfTopic_copy (sbfTopic topic);

/*!
   \brief compares two topics and returns
       =>  0 if they are equal,
       => -1 if lhs is less than rhs,
       =>  1 if rhs is less than lhs.
   \param lhs left topic handler.
   \param rhs right topic handler.
   \return
       =>  0 if they are equal,
       => -1 if lhs is less than rhs,
       =>  1 if rhs is less than lhs.
*/
int sbfTopic_compare (sbfTopic lhs, sbfTopic rhs);

/*!
   \brief returns the type of topic.
   \param topic the topic handler.
   \return the type of topic.
*/
sbfTopicType sbfTopic_getType (sbfTopic topic);

/*!
   \brief returns the name of the topic.
   \param topic the topic handler.
   \return the name of the topic.
*/
const char* sbfTopic_getTopic (sbfTopic topic);

/*!
   \brief Returns the first topic name.
   \param topic the topic handler.
   \return the first topic name.
*/
const char* sbfTopic_getFirst (sbfTopic topic);

/*!
   \brief Returns the second topic name.
   \param topic the topic handler.
   \return the second topic name.
*/
const char* sbfTopic_getSecond (sbfTopic topic);

/*!
   \brief Returns the third topic name.
   \param topic the topic handler.
   \return the third topic name.
*/
const char* sbfTopic_getThird (sbfTopic topic);

SBF_END_DECLS

#endif /* _SBF_TOPIC_H_ */
