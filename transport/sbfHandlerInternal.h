#ifndef _SBF_HANDLER_H_
#define _SBF_HANDLER_H_

#include "sbfCommon.h"
#include "sbfTport.h"

SBF_BEGIN_DECLS

typedef void* sbfHandler;
typedef void* sbfHandlerStream;
typedef void* sbfHandlerHandle;

/* Must be fired by the handler on the event thread given to AddStream. */
typedef void (*sbfHandlerAddStreamCompleteCb) (sbfHandlerHandle handle,
                                               sbfError error,
                                               void* closure);

typedef sbfHandler (*sbfHandlerCreateFn) (sbfTport tport,
                                          sbfKeyValue properties);
typedef void (*sbfHandlerDestroyFn) (sbfHandler handler);

/*
 * FindStream and AddStream can come off any thread, RemoveStream off the event
 * thread. All three are synchronized by the caller, but the handler must
 * guarantee that the handle given to AddStream is available immediately from
 * FindStream, even if the AddStreamComplete callback has not yet been fired.
 */
typedef sbfHandlerHandle (*sbfHandlerFindStreamFn) (sbfHandler handler,
                                                    sbfTopic topic);
typedef sbfHandlerStream (*sbfHandlerAddStreamFn) (
                                              sbfHandler handler,
                                              sbfTopic topic,
                                              sbfMwThread thread,
                                              sbfHandlerHandle handle,
                                              sbfHandlerAddStreamCompleteCb cb,
                                              void* closure);
typedef void (*sbfHandlerRemoveStreamFn) (sbfHandlerStream stream);

typedef sbfBuffer (*sbfHandlerGetBufferFn) (sbfHandlerStream stream,
                                           size_t size);
typedef void (*sbfHandlerSendBufferFn) (sbfHandlerStream stream,
                                        sbfBuffer buffer);

typedef struct
{
    sbfHandlerCreateFn       mCreate;
    sbfHandlerDestroyFn      mDestroy;

    sbfHandlerFindStreamFn   mFindStream;
    sbfHandlerAddStreamFn    mAddStream;
    sbfHandlerRemoveStreamFn mRemoveStream;

    sbfHandlerGetBufferFn    mGetBuffer;
    sbfHandlerSendBufferFn   mSendBuffer;
} sbfHandlerTable;

sbfHandlerTable* sbfHandler_load (const char* type);

void sbfHandler_message (sbfHandlerHandle handle, sbfBuffer buffer);

#endif /* _SBF_HANDLER_H_ */
