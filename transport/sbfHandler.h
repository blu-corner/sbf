/*!
   \file sbfHandler.h
   \brief This file declares structures and functions for
   different type of handlers.
   \copyright © Copyright 2016 Neueda all rights reserved.
*/
#ifndef _SBF_HANDLER_H_
#define _SBF_HANDLER_H_

#include "sbfCommon.h"
#include "sbfTport.h"

SBF_BEGIN_DECLS

typedef void* sbfHandler;
typedef void* sbfHandlerStream;
typedef void* sbfHandlerHandle;

/*!
   \brief Callback invoked when add stream is completed.
   \param handle transport handler.
   \param error the error during add stream operation if any.
   \param closure Data linked to the transport handler.
*/
typedef void (*sbfHandlerAddStreamCompleteCb) (sbfHandlerHandle handle,
                                               sbfError error,
                                               void* closure);

/*!
   \brief Creates a transport handler for a given port and properties.
   \param tport the port used to handle the transport.
   \return a transport handler for a given port and properties.
*/
typedef sbfHandler (*sbfHandlerCreateFn) (sbfTport tport,
                                          sbfKeyValue properties);

/*!
   \brief Releases all the resources allocated by the create function.
   \param handle transport handler.
*/
typedef void (*sbfHandlerDestroyFn) (sbfHandler handler);

/*
 */
/*!
   \brief
   FindStream and AddStream can come off any thread, RemoveStream off the event
   thread. All three are synchronized by the caller, but the handler must
   guarantee that the handle given to AddStream is available immediately from
   FindStream, even if the AddStreamComplete callback has not yet been fired.
   \param handle transport handler.
   \param topic the topic related to the stream.
   \return the transport handler.
*/
typedef sbfHandlerHandle (*sbfHandlerFindStreamFn) (sbfHandler handler,
                                                    sbfTopic topic);
/*!
   \brief Adds a stream to a given transport handler, topic.
   \param handle transport handler.
   \param topic the topic to be linked to the stream.
   \param thread a thread.
   \param handle a handler.
   \param cb a callback that will be invoked when stream is completed.
   \return returns a stream handler.
*/
typedef sbfHandlerStream (*sbfHandlerAddStreamFn) (
                                              sbfHandler handler,
                                              sbfTopic topic,
                                              sbfMwThread thread,
                                              sbfHandlerHandle handle,
                                              sbfHandlerAddStreamCompleteCb cb,
                                              void* closure);

/*!
   \brief Removes a stream.
   \param stream handler.
*/
typedef void (*sbfHandlerRemoveStreamFn) (sbfHandlerStream stream);

typedef sbfBuffer (*sbfHandlerGetBufferFn) (sbfHandlerStream stream,
                                            size_t size);
typedef void (*sbfHandlerSendBufferFn) (sbfHandlerStream stream,
                                        sbfBuffer buffer);

/* Handler will never be given a packet bigger than mPacketSize. */
typedef struct
{
    uint16_t                 mPacketSize;

    sbfHandlerCreateFn       mCreate;
    sbfHandlerDestroyFn      mDestroy;

    sbfHandlerFindStreamFn   mFindStream;
    sbfHandlerAddStreamFn    mAddStream;
    sbfHandlerRemoveStreamFn mRemoveStream;

    sbfHandlerGetBufferFn    mGetBuffer;
    sbfHandlerSendBufferFn   mSendBuffer;
} sbfHandlerTable;

/*!
   \brief Helper function that loads a handler given its type
   \param log a log handler.
   \param type the type of connection.
   \return a pointer to handler table.
*/
sbfHandlerTable* sbfHandler_load (sbfLog log, const char* type);

/*!
   \brief Returns .the necessary size for a given buffer.
   \param data pointer to the buffer containing the payload.
   \param size size in bytes of the buffer.
   \return the necessary size for a given buffer
*/
size_t sbfHandler_size (void* data, size_t size);


void sbfHandler_packet (sbfHandlerHandle handle, sbfBuffer buffer);

#endif /* _SBF_HANDLER_H_ */
