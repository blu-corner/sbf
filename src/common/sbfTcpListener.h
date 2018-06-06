
#ifndef _SBF_TCP_LISTENER_H_
#define _SBF_TCP_LISTENER_H_

#include "sbfCommon.h"

SBF_BEGIN_DECLS

struct sbfMwThreadImpl;
struct sbfQueueImpl;
struct sbfTcpConnectionImpl;

typedef struct sbfTcpListenerImpl* sbfTcpListener;

typedef void (*sbfTcpListenerReadyCb) (sbfTcpListener tl, void* closure);
typedef void (*sbfTcpListenerAcceptCb) (sbfTcpListener tl,
                                        struct sbfTcpConnectionImpl* tc,
                                        void* closure);

sbfTcpListener sbfTcpListener_create (sbfLog log,
                                      struct sbfMwThreadImpl* thread,
                                      struct sbfQueueImpl* queue,
                                      sbfTcpConnectionAddress* address,
                                      int isUnix,
                                      sbfTcpListenerReadyCb readyCb,
                                      sbfTcpListenerAcceptCb acceptCb,
                                      void* closure);

void sbfTcpListener_destroy (sbfTcpListener tl);

int sbfTcpListener_isUnix (sbfTcpListener t1);

SBF_END_DECLS

#endif /* _SBF_TCP_LISTENER_H_ */
