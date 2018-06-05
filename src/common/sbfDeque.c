#include "sbfDeque.h"
#include "sbfDequePrivate.h"

sbfDeque
sbfDeque_create (void)
{
    sbfDeque dq;

    dq = xcalloc (1, sizeof *dq);
    dq->mPool = sbfPool_create (sizeof (struct sbfDequeItemImpl));
    TAILQ_INIT (&dq->mList);

    return dq;
}

void
sbfDeque_destroy (sbfDeque dq)
{
    sbfPool_destroy (dq->mPool);
    free (dq);
}

void
sbfDeque_pushBack (sbfDeque dq, void* vp)
{
    sbfDequeItem dqe;

    dqe = sbfPool_get (dq->mPool);
    dqe->mPointer = vp;
    TAILQ_INSERT_TAIL (&dq->mList, dqe, mListEntry);
}

void
sbfDeque_pushFront (sbfDeque dq, void* vp)
{
    sbfDequeItem dqe;

    dqe = sbfPool_get (dq->mPool);
    dqe->mPointer = vp;
    TAILQ_INSERT_HEAD (&dq->mList, dqe, mListEntry);
}

void*
sbfDeque_popBack (sbfDeque dq)
{
    sbfDequeItem dqe;
    void*        vp;

    dqe = TAILQ_LAST (&dq->mList, sbfDequeItemListImpl);
    if (dqe == NULL)
        return NULL;
    vp = dqe->mPointer;

    TAILQ_REMOVE (&dq->mList, dqe, mListEntry);
    sbfPool_put (dqe);

    return vp;
}

void*
sbfDeque_popFront (sbfDeque dq)
{
    sbfDequeItem dqe;
    void*        vp;

    dqe = TAILQ_FIRST (&dq->mList);
    if (dqe == NULL)
        return NULL;
    vp = dqe->mPointer;

    TAILQ_REMOVE (&dq->mList, dqe, mListEntry);
    sbfPool_put (dqe);

    return vp;
}

void*
sbfDeque_back (sbfDeque dq)
{
    sbfDequeItem dqe;

    dqe = TAILQ_LAST (&dq->mList, sbfDequeItemListImpl);
    return dqe->mPointer;
}

void*
sbfDeque_front (sbfDeque dq)
{
    sbfDequeItem dqe;

    dqe = TAILQ_FIRST (&dq->mList);
    return dqe->mPointer;
}

