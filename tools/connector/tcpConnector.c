#include "sbfCommon.h"
#include "sbfMw.h"
#include "sbfTcpConnection.h"
#include "sbfTport.h"

/*!
 \iriefi Callback from thread to dispatch the items in the queue.
 \param closure data linked to the thread callback.
*/
static void*
dispatchCb (void* closure)
{
    sbfQueue_dispatch (closure);
    return NULL;
}

void onConnectionReady (sbfTcpConnection tc, void* closure)
{
    printf("on-ready!\n");
}

void onConnectionError (sbfTcpConnection tc, void* closure)
{
    printf("on-error!\n");
}

size_t onConnectionRead (sbfTcpConnection tc,
                         void* data,
                         size_t size,
                         void* closure)
{
    printf("on-read!\n");
    return size;
}

int
main (int argc, char** argv)
{
    sbfMw               mw;
    sbfQueue            queue;
    sbfThread           t;
    sbfKeyValue         properties;
    sbfLog              log;
    sbfTcpConnection    connection;

    log = sbfLog_create (NULL, "sbf");
    sbfLog_setLevel (log, SBF_LOG_OFF);

    properties = sbfKeyValue_create ();
    mw = sbfMw_create (log, properties);

    queue = sbfQueue_create (mw, "default");
    sbfThread_create (&t, dispatchCb, queue);

    sbfTcpConnectionAddress address;
    memset(&address, 0, sizeof(address));
    address.sun.sun_family = AF_UNIX;
    strncpy(address.sun.sun_path, "/tmp/test.sock", sizeof(address.sun.sun_path) - 1);
    int isUnix = 1;
    
    connection = sbfTcpConnection_create (log,
                                          sbfMw_getDefaultThread (mw),
                                          queue,
                                          &address,
                                          isUnix,
                                          onConnectionReady,
                                          onConnectionError,
                                          onConnectionRead,
                                          NULL);
    if (connection == NULL)
    {
        fprintf (stderr, "failed to create connection to real time host");
    }
    
    exit (0);
}

