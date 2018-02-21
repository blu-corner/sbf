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

/*!
 \brief Callback that replies with the number of request performed.
 \param sub  pointer to subscriber.
 \param req pointer to request.
 \param buffer data being requested.
 \param closure the data linked to this callback.
*/
static void
requestCb (sbfRequestSub sub, sbfRequest req, sbfBuffer buffer, void* closure)
{
  uint64_t*       value;
  static uint64_t next;

  if (sbfBuffer_getSize (buffer) < sizeof *value)
      return;
  value = sbfBuffer_getData (buffer);
  printf ("<-- request %llu\n", (unsigned long long)*value);

  next++;
  sbfRequest_reply (req, &next, sizeof next);
  printf ("--> reply %llu\n", (unsigned long long)next);
}

/*!
 \brief Print out the description of this command.
 \param argv0 the name of the command.
*/
static void
usage (const char* argv0)
{
    fprintf (stderr,
           "usage: %s "
           "[-h handler] "
           "[-i interface] "
           "[-t topic] "
           "[-v level]\n",
           argv0);

    exit (1);
}

int
main (int argc, char** argv)
{
    sbfMw               mw;
    sbfQueue            queue;
    sbfThread           t;
    sbfTport            tport;
    sbfKeyValue         properties;
    sbfLog              log;
    sbfTcpConnection    connection;
    int                 opt;
    const char*         handler = "tcp";
    const char*         interf = "eth0"; //??



    log = sbfLog_create (NULL, "sbf");
    sbfLog_setLevel (log, SBF_LOG_OFF);

//??what is needed
/*  while ((opt = getopt (argc, argv, "h:i:t:v:")) != -1) {
        switch (opt) {
        case 'h':
            handler = optarg;
            break;
        case 'i':
            interf = optarg;
        break;
        case 't':
            topic = optarg;
        break;
        case 'v':
            sbfLog_setLevel (log, sbfLog_levelFromString (optarg, NULL));
            break;
        default:
            usage (argv0);
        }
    }
    argc -= optind;
    argv += optind;
    if (argc != 0)
        usage (argv0);
*/

    properties = sbfKeyValue_create ();
    sbfKeyValue_put (properties, "transport.default.type", handler);
    sbfKeyValue_put (properties, "transport.default.tcp.interface", interf);

    mw = sbfMw_create (log, properties);

    queue = sbfQueue_create (mw, "default");
    sbfThread_create (&t, dispatchCb, queue);

    tport = sbfTport_create (mw, "default", SBF_MW_ALL_THREADS);

//  sbfRequestSub_create (tport, queue, topic, NULL, requestCb, NULL);

    connection = sbfTcpConnection_create (log,
                                        sbfMw_getDefaultThread (mw)
                                        queue,
                                         //HOST DETAILS from sbfKEYVAL?
                                         );
    if (connection == NULL)
    {
        log->err ("failed to create connection to real time host");
    }
    exit (0);
}


