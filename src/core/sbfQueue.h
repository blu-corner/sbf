/*!
   \file sbfQueue.h
   \brief This file declares the interface for handling queues.
   \Copyright 2014-2018 Neueda Ltd.
*/
#ifndef _SBF_QUEUE_H_
#define _SBF_QUEUE_H_

#include "sbfCommon.h"

SBF_BEGIN_DECLS

struct sbfMwImpl;

/** \brief Declaration of the queue handler */
typedef struct sbfQueueImpl* sbfQueue;

/** \brief Declaration of the queue item handler */
typedef struct sbfQueueItemImpl* sbfQueueItem;

/*!
   \brief callback invoked when item needs to be processed.
   \param item the item handler.
   \param closure the user data associated to the queue item.
*/
typedef void (*sbfQueueCb) (sbfQueueItem item, void* closure);

// TODO: Shouldn't this helper function be in middleware??
/*!
   \brief Returns a queue handler for the given name.
   \param mw middleware handler.
   \param name the name of the topic assigned to the queue.
   \return a queue handler for the given name.
*/
sbfQueue sbfQueue_create (struct sbfMwImpl* mw, const char* name);

/*!
   \brief Releases the memory allocated to the given queue handler.
   \param queue the queue handler.
*/
void sbfQueue_destroy (sbfQueue queue);

/*!
   \brief Returns the name of the topic assigned to the queue.
   \param queue the queue handler.
   \return null terminated string containing the name of the queue (topic).
*/
const char* sbfQueue_getName (sbfQueue queue);

/*!
   \brief Adds an item to the queue.
   \param queue the queue handler.
   \param cb the callback indicating the item has been queued.
   \param closure the user data associated to the queue item.
*/
void sbfQueue_enqueue (sbfQueue queue, sbfQueueCb cb, void* closure);

/*!
   \brief Dispatch all of the items that belongs to the given queue handler.
   \param queue the queue handler.
*/
void sbfQueue_dispatch (sbfQueue queue);

/*!
   \brief returns the item from the queue by adding the callback and user data.
   \param queue the queue handler.
   \param cb the callback indicating the item has been queued.
   \param closure the user data associated to the queue item.
   \return the item from the queue.
*/
sbfQueueItem sbfQueue_getItem (sbfQueue queue, sbfQueueCb cb, void* closure);
void sbfQueue_enqueueItem (sbfQueue queue, sbfQueueItem item);

/*
* Each item has a small data buffer associated with it, to avoid having to
* allocate a container object for callbacks which need multiple arguments.
*/
 /*!
    \brief Returns the data associated to the given queue item.
    \param item the queue item handler.
    \return the data associated to a queue item.
    \see sbfQueue_getItemDataSize
 */
void* sbfQueue_getItemData (sbfQueueItem item);

/*!
   \brief Returns the size of the given queue item.
   \param item the queue item handler.
   \return the size of the given queue item.
   \see sbfQueue_getItemData
*/
size_t sbfQueue_getItemDataSize (sbfQueueItem item);

SBF_END_DECLS

#endif /* _SBF_QUEUE_H_ */
