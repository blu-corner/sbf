#ifndef _SBF_DEQUE_H_
#define _SBF_DEQUE_H_

#include "sbfCommon.h"

SBF_BEGIN_DECLS

typedef struct sbfDequeImpl* sbfDeque;

sbfDeque sbfDeque_create (void);
void sbfDeque_destroy (sbfDeque dq);

void sbfDeque_pushBack (sbfDeque dq, void* vp);
void sbfDeque_pushFront (sbfDeque dq, void* vp);

void* sbfDeque_popBack (sbfDeque dq);
void* sbfDeque_popFront (sbfDeque dq);

void* sbfDeque_back (sbfDeque dq);
void* sbfDeque_front (sbfDeque dq);

SBF_END_DECLS

#endif /* _SBF_DEQUE_H_ */
