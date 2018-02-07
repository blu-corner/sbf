/*!
   \file sbfTcpConnection.h
   \brief This file declares the structures and functions to handle
   TCP connections.
   \copyright © Copyright 2016 Neueda all rights reserved.
 */

#ifndef _SBF_TCP_CONNECTION_H_
#define _SBF_TCP_CONNECTION_H_

#include "sbfBuffer.h"
#include "sbfCommon.h"

SBF_BEGIN_DECLS

struct sbfMwThreadImpl;
struct sbfQueueImpl;

/*! Declares the TCP connection handler. */
typedef struct sbfTcpConnectionImpl* sbfTcpConnection;

/*! Callback invoked when TCP connection is ready. */
typedef void (*sbfTcpConnectionReadyCb) (sbfTcpConnection tc, void* closure);

/*! Callback invoked when TCP connection error happens. */
typedef void (*sbfTcpConnectionErrorCb) (sbfTcpConnection tc, void* closure);

/*! Read callback should return how much of buffer was used. */
typedef size_t (*sbfTcpConnectionReadCb) (sbfTcpConnection tc,
                                          void* data,
                                          size_t size,
                                          void* closure);

/*!
   Creates a TCP connection handler.
   \param log the log handler.
   \param thread a thread to handle TCP connections.
   \param queue a queue to dispatch TCP messages.
   \param sin Information about the socket connection.
   \param readyCb a ready callback to be notified when the connection
   is ready.
   \param errorCb an error callback to notify when an error happened.
   \param readCb a read callback to inform about the data read operations.
   \param closure user data linked to the TCP connection.
   \return a TCP connection handler.
 */
sbfTcpConnection sbfTcpConnection_create (sbfLog log,
                                          struct sbfMwThreadImpl* thread,
                                          struct sbfQueueImpl* queue,
                                          struct sockaddr_in* sin,
                                          sbfTcpConnectionReadyCb readyCb,
                                          sbfTcpConnectionErrorCb errorCb,
                                          sbfTcpConnectionReadCb readCb,
                                          void* closure);

/*!
   Creates a TCP connection handler.
   \param log the log handler.
   \param thread a thread to handle TCP connections.
   \param queue a queue to dispatch TCP messages.
   \param sin Information about the socket connection.
   \param readyCb a ready callback to be notified when the connection
   is ready.
   \param errorCb an error callback to notify when an error happened.
   \param readCb a read callback to inform about the data read operations.
   \param closure user data linked to the TCP connection.
   \return a TCP connection handler.
 */
sbfTcpConnection sbfTcpConnection_createUnix (sbfLog log,
                                              struct sbfMwThreadImpl* thread,
                                              struct sbfQueueImpl* queue,
                                              struct sockaddr_un* sin,
                                              sbfTcpConnectionReadyCb readyCb,
                                              sbfTcpConnectionErrorCb errorCb,
                                              sbfTcpConnectionReadCb readCb,
                                              void* closure);

/*!
   Releases the resources allocated by the TCP connection.
   \param tc the connection handler.
 */
void sbfTcpConnection_destroy (sbfTcpConnection tc);

/*!
   Accepts an incoming connection and link it with the given callbacks.
   \param tc the connection handler.
   \param thread a thread to handle TCP connections.
   \param queue a queue to dispatch TCP messages.
   \param readyCb a ready callback to be notified when the connection.
   is ready.
   \param errorCb an error callback to notify when an error happened.
   \param readCb a read callback to inform about the data read operations.
   \param closure user data linked to the TCP connection.
   \return 0 if successful, error code otherwise.
 */
sbfError sbfTcpConnection_accept (sbfTcpConnection tc,
                                  struct sbfMwThreadImpl* thread,
                                  struct sbfQueueImpl* queue,
                                  sbfTcpConnectionReadyCb readyCb,
                                  sbfTcpConnectionErrorCb errorCb,
                                  sbfTcpConnectionReadCb readCb,
                                  void* closure);

/*!
   Sends a binary data through the TCP connection.
   \param tc the TCP connection's handler.
   \param data the data to be sent through the TCP connection.
   \param size the size of the data.
 */
void sbfTcpConnection_send (sbfTcpConnection tc,
                            const void* data,
                            size_t size);

/*!
   Sends a buffer through the TCP connection.
   \param tc the TCP connection's handler.
   \param buffer the buffer to be sent through the TCP connection.
 */
void sbfTcpConnection_sendBuffer (sbfTcpConnection tc, sbfBuffer buffer);

/*!
   Returns the connection details for a given TCP connection handler.
   \param tc the TCP connection's handler.
   \return the connection details for a given TCP connection handler.
 */
struct sockaddr_in* sbfTcpConnection_getPeer (sbfTcpConnection tc);

struct sockaddr_un* sbfTcpConnection_getPeerUnix (sbfTcpConnection tc);

bool sbfTcpConnection_isUnix (sbfTcpConnection tc);

SBF_END_DECLS

#endif /* _SBF_TCP_CONNECTION_H_ */
