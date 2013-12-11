#ifndef _SBF_TCP_CONNECTION_H_
#define _SBF_TCP_CONNECTION_H_

#include "sbfBuffer.h"
#include "sbfCommon.h"

SBF_BEGIN_DECLS

struct sbfMwThreadImpl;
struct sbfQueueImpl;

typedef struct sbfTcpConnectionImpl* sbfTcpConnection;

/* Read callback should return how much of buffer was used. */
typedef void (*sbfTcpConnectionReadyCb) (sbfTcpConnection tc, void* closure);
typedef size_t (*sbfTcpConnectionReadCb) (sbfTcpConnection tc,
                                          sbfBuffer buffer,
                                          void* closure);

sbfTcpConnection sbfTcpConnection_create (struct sbfMwThreadImpl* thread,
                                          struct sbfQueueImpl* queue,
                                          const char* address,
                                          uint16_t port,
                                          sbfTcpConnectionReadyCb readyCb,
                                          sbfTcpConnectionReadCb readCb,
                                          void* closure);
void sbfTcpConnection_destroy (sbfTcpConnection tc);

sbfError sbfTcpConnection_accept (sbfTcpConnection tc,
                                  struct sbfMwThreadImpl* thread,
                                  struct sbfQueueImpl* queue,
                                  sbfTcpConnectionReadyCb readyCb,
                                  sbfTcpConnectionReadCb readCb,
                                  void* closure);

void sbfTcpConnection_send (sbfTcpConnection tc, void* data, size_t size);
void sbfTcpConnection_sendBuffer (sbfTcpConnection tc, sbfBuffer buffer);

SBF_END_DECLS

#endif /* _SBF_TCP_CONNECTION_H_ */
