#ifndef _SBF_TOPIC_H_
#define _SBF_TOPIC_H_

#include "sbfCommon.h"

SBF_BEGIN_DECLS

typedef struct sbfTopicImpl* sbfTopic;

#define SBF_TOPIC_SIZE_LIMIT 255

typedef enum
{
    SBF_TOPIC_PUB,
    SBF_TOPIC_SUB
} sbfTopicType;

sbfTopic sbfTopic_create (sbfTopicType type, const char* string);
void sbfTopic_destroy (sbfTopic topic);

sbfTopic sbfTopic_copy (sbfTopic topic);

int sbfTopic_compare (sbfTopic lhs, sbfTopic rhs);

sbfTopicType sbfTopic_getType (sbfTopic topic);
const char* sbfTopic_getTopic (sbfTopic topic);

const char* sbfTopic_getFirst (sbfTopic topic);
const char* sbfTopic_getSecond (sbfTopic topic);
const char* sbfTopic_getThird (sbfTopic topic);

SBF_END_DECLS

#endif /* _SBF_TOPIC_H_ */
