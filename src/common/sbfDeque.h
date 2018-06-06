/*!
   \file sbfDeque.h
   \brief This file declares the structures and functions to handle a queue.
   \Copyright 2014-2018 Neueda Ltd.
 */

#ifndef _SBF_DEQUE_H_
#define _SBF_DEQUE_H_

#include "sbfCommon.h"

SBF_BEGIN_DECLS

/*! Declares the queue handller */
typedef struct sbfDequeImpl* sbfDeque;

/*!
   \brief Creates a queue.
   \return the created queue's handler.
   \see sbfDeque_destroy
 */
sbfDeque sbfDeque_create (void);

/*!
   \brief Releases all the resources allocated by the queue.
   \param dq the queue's handler.
   \see sbfDeque_create
 */
void sbfDeque_destroy (sbfDeque dq);

/*!
   \brief Pushes an element at the end of the queue.
   \param dq the queue's handler.
   \param vp the data to be pushed into the queue.
 */
void sbfDeque_pushBack (sbfDeque dq, void* vp);

/*!
   \brief Pushes an element in front of the queue.
   \param dq the queue's handler.
   \param vp the data to be pushed into the queue.
 */
void sbfDeque_pushFront (sbfDeque dq, void* vp);

/*!
   \brief Extracts an element from the back of the queue and return it.
   \param dq the queue's handler.
   \return the element that was at the back of the queue.
 */
void* sbfDeque_popBack (sbfDeque dq);

/*!
   \brief Extracts an element from the front of the queue and return it.
   \param dq the queue's handler.
   \return the element that was at the front of the queue.
 */
void* sbfDeque_popFront (sbfDeque dq);

/*!
   \brief Returns the element that was at the back of the queue but does not extract
   it from the queue.
   \param dq the queue's handler.
   \return the element that was at the back of the queue but does not extract.
 */
void* sbfDeque_back (sbfDeque dq);

/*!
   \brief Returns the element that was at the front of the queue but does not extract
   it from the queue.
   \param dq the queue's handler.
   \return the element that was at the front of the queue.
 */
void* sbfDeque_front (sbfDeque dq);

SBF_END_DECLS

#endif /* _SBF_DEQUE_H_ */
