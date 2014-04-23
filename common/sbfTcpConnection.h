#ifndef _SBF_TCP_CONNECTION_H_
#define _SBF_TCP_CONNECTION_H_

#include "sbfBuffer.h"
#include "sbfCommon.h"

SBF_BEGIN_DECLS

struct sbfMwThreadImpl;
struct sbfQueueImpl;

typedef struct sbfTcpConnectionImpl* sbfTcpConnection;

typedef void (*sbfTcpConnectionReadyCb) (sbfTcpConnection tc, void* closure);
typedef void (*sbfTcpConnectionErrorCb) (sbfTcpConnection tc, void* closure);

/* Read callback should return how much of buffer was used. */
typedef size_t (*sbfTcpConnectionReadCb) (sbfTcpConnection tc,
                                          void* data,
                                          size_t size,
                                          void* closure);

sbfTcpConnection sbfTcpConnection_create (struct sbfMwThreadImpl* thread,
                                          struct sbfQueueImpl* queue,
                                          const char* address,
                                          uint16_t port,
                                          sbfTcpConnectionReadyCb readyCb,
                                          sbfTcpConnectionErrorCb errorCb,
                                          sbfTcpConnectionReadCb readCb,
                                          void* closure);
void sbfTcpConnection_destroy (sbfTcpConnection tc);

sbfError sbfTcpConnection_accept (sbfTcpConnection tc,
                                  struct sbfMwThreadImpl* thread,
                                  struct sbfQueueImpl* queue,
                                  sbfTcpConnectionReadyCb readyCb,
                                  sbfTcpConnectionErrorCb errorCb,
                                  sbfTcpConnectionReadCb readCb,
                                  void* closure);

void sbfTcpConnection_send (sbfTcpConnection tc, const void* data, size_t size);
void sbfTcpConnection_sendBuffer (sbfTcpConnection tc, sbfBuffer buffer);

struct sockaddr_in* sbfTcpConnection_getPeer (sbfTcpConnection tc);

SBF_END_DECLS

#endif /* _SBF_TCP_CONNECTION_H_ */
