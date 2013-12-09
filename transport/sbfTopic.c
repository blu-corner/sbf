#include "sbfTport.h"
#include "sbfTportPrivate.h"

sbfTopic
sbfTopic_create (sbfTopicType type, const char* string)
{
    sbfTopic topic;
    size_t   size;
    char*    first;
    char*    second;
    char*    cp;

    size = strlen (string) + 1;

    topic = xcalloc (1, sizeof *topic);
    topic->mType = type;

    cp = first = xmalloc (size);
    while (*string != '\0' && *string != '/')
        *cp++ = *string++;
    *cp = '\0';
    if (*string == '\0')
    {
        topic->mFirst = xstrdup ("");
        topic->mSecond = xstrdup ("");
        topic->mThird = first;
        goto out;
    }
    string++;

    cp = second = xmalloc (size);
    while (*string != '\0' && *string != '/')
        *cp++ = *string++;
    *cp = '\0';
    if (*string == '\0')
    {
        topic->mFirst = xstrdup ("");
        topic->mSecond = first;
        topic->mThird = second;
        goto out;
    }
    string++;

    topic->mFirst = first;
    topic->mSecond = second;
    topic->mThird = xstrdup (string);

out:
    if (asprintf (&cp,
                  "%s/%s/%s",
                  topic->mFirst,
                  topic->mSecond,
                  topic->mThird) == -1)
        sbfFatal ("out of memory");
    topic->mTopic = cp;

    if (strlen (topic->mTopic) > SBF_TOPIC_SIZE_LIMIT)
    {
        sbfTopic_destroy (topic);
        return NULL;
    }
    return topic;
}

void
sbfTopic_destroy (sbfTopic topic)
{
    free ((void*)topic->mThird);
    free ((void*)topic->mSecond);
    free ((void*)topic->mFirst);

    free ((void*)topic->mTopic);
    free (topic);
}

sbfTopic
sbfTopic_copy (sbfTopic topic)
{
    sbfTopic copy;

    copy = calloc (1, sizeof *copy);
    copy->mType = topic->mType;
    copy->mTopic = xstrdup (topic->mTopic);

    copy->mFirst = xstrdup (topic->mFirst);
    copy->mSecond = xstrdup (topic->mSecond);
    copy->mThird = xstrdup (topic->mThird);

    return copy;
}

int
sbfTopic_compare (sbfTopic lhs, sbfTopic rhs)
{
    if (lhs->mType != rhs->mType)
    {
        if (lhs->mType == SBF_TOPIC_PUB)
            return -1;
        return 1;
    }
    return strcmp (lhs->mTopic, rhs->mTopic);
}

sbfTopicType
sbfTopic_getType (sbfTopic topic)
{
    return topic->mType;
}

const char*
sbfTopic_getTopic (sbfTopic topic)
{
    return topic->mTopic;
}

const char*
sbfTopic_getFirst (sbfTopic topic)
{
    return topic->mFirst;
}

const char*
sbfTopic_getSecond (sbfTopic topic)
{
    return topic->mSecond;
}

const char*
sbfTopic_getThird (sbfTopic topic)
{
    return topic->mThird;
}
