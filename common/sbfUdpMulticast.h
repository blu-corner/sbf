/*!
   \file sbfUdpMulticast.h
   \brief This file declares the structures and functions to handle
   UDP multicast connections.
   \copyright © Copyright 2016 Neueda all rights reserved.
 */

#ifndef _SBF_UDP_MULTICAST_H_
#define _SBF_UDP_MULTICAST_H_

#include "sbfBuffer.h"
#include "sbfCommon.h"

SBF_BEGIN_DECLS

/*! Declares the UPD multicast's handler. */
typedef struct sbfUdpMulticastImpl* sbfUdpMulticast;

/*! Maximum multicast size limit */
#define SBF_UDP_MULTICAST_SIZE_LIMIT 65536

/*! UDP multicast types. */
typedef enum
{
    SBF_UDP_MULTICAST_SEND,
    SBF_UDP_MULTICAST_LISTEN
} sbfUdpMulticastType;

/*! Callback invoked when reading from UDP multicast connection. */
typedef sbfError (*sbfUdpMulticastReadCb) (sbfUdpMulticast s,
                                           sbfBuffer buffer,
                                           void* closure);

/*!
   \brief Creates a UDP multicast connection.
   \param type the type of UDP multicast connection.
   \param interf the interface.
   \param address the connection details.
   \return a UDP multicast connection's handler.
 */
sbfUdpMulticast sbfUdpMulticast_create (sbfUdpMulticastType type,
                                        uint32_t interf,
                                        struct sockaddr_in* address);
/*!
   \brief Releases the memory allocated by the UDP multicast's handler.
   \param s the UDP multicast handler.
 */
void sbfUdpMulticast_destroy (sbfUdpMulticast s);

/*!
   \brief Returns the socket for a UDP multicast connection.
   \param s the UDP multicast handler.
   \return the socket for a UDP multicast connection.
 */
sbfSocket sbfUdpMulticast_getSocket (sbfUdpMulticast s);

/*! Macro to send a buffer. */
#define sbfUdpMulticast_sendBuffer(s, b) \
    sbfUdpMulticast_send (s, sbfBuffer_getData (b), sbfBuffer_getSize (b))

/*!
   \brief Sends a buffer through the UDP connection.
   \param s the UDP multicast handler.
   \param buf the buffer to be sent.
   \param len the size of the buffer in bytes.
   \return 0 successful, error code otherwise.
 */
sbfError sbfUdpMulticast_send (sbfUdpMulticast s, const void* buf, size_t len);

/*!
   \brief reads from a UDP multicast connection.
   \param s the UDP multicast handler.
   \param cb the read callback.
   \param closure user data linked to the read operation.
   \param limit number of internal read operations to be performed.
   \return 0 if successfull, error code otherwise.
 */
sbfError sbfUdpMulticast_read (sbfUdpMulticast s,
                               sbfUdpMulticastReadCb cb,
                               void* closure,
                               u_int limit);

SBF_END_DECLS

#endif /* _SBF_UDP_MULTICAST_H_ */
