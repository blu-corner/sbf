#ifndef _SBF_TOPIC_PRIVATE_H_
#define _SBF_TOPIC_PRIVATE_H_

SBF_BEGIN_DECLS

struct sbfTopicImpl
{
    sbfTopicType mType;
    const char*  mTopic;

    const char*  mFirst;
    const char*  mSecond;
    const char*  mThird;
};

SBF_END_DECLS

#endif /* _SBF_TOPIC_PRIVATE_H_ */
