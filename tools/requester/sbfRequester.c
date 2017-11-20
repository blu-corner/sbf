/*!
   \file sbfRequester.c
   \brief TBD
   \copyright © Copyright 2016 Neueda all rights reserved.
*/
#include "sbfCommon.h"
#include "sbfMw.h"
#include "sbfRequestReply.h"
#include "sbfTport.h"

/*!
   \brief Callback that will dispatch queue items.
   \param closure data containing the queue.
   \return pointer to data, in this case .
*/
static void*
dispatchCb (void* closure)
{
    sbfQueue_dispatch (closure);
    return NULL;
}

/*!
   \brief Callback that receives replier responses.
   \param sub  pointer to subscriber.
   \param req pointer to request.
   \param buffer data being requested.
   \param closure the data linked to this callback.
   \return "Return of the function"
*/
static void
replyCb (sbfRequestPub pub, sbfRequest req, sbfBuffer buffer, void* closure)
{
    uint64_t* value;

    if (sbfBuffer_getSize (buffer) < sizeof *value)
        return;
    value = sbfBuffer_getData (buffer);
    printf ("<-- reply %llu\n", (unsigned long long)*value);
}

/*!
   \brief Timer callback to process a request.
   \param timer callback fired by timer.
   \param closure the publisher.
*/
static void
timerCb (sbfTimer timer, void* closure)
{
    static uint64_t next;

    next++;
    sbfRequestPub_send (closure, &next, sizeof next, replyCb, NULL);
    printf ("--> request %llu\n", (unsigned long long)next);
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

/*!
   \brief This function is the main entry point for the requester command.
   You can specify the following parameters to run the requester:
   "[-h handler] " the type of connection handler: tcp or udp.
   "[-i interface] " the connection interface e.g. eth0.
   "[-t topic] " the name of the topic where messages are published.
   "[-v level]\n" log severity level.
   \param argc The number of arguments passed to the command.
   \param argv array of null terminated strings.
   \return Error status: 0 for Successfull another indicates an error code.
   \return "Return of the function"
*/
int
main (int argc, char** argv)
{
    const char*   argv0 = argv[0];
    sbfMw         mw;
    sbfQueue      queue;
    sbfThread     t;
    sbfTport      tport;
    sbfKeyValue   properties;
    sbfLog        log;
    sbfRequestPub pub;
    int           opt;
    const char*   topic = "OUT";
    const char*   handler = "udp";
    const char*   interf = "eth0";

    log = sbfLog_create (NULL, "%s", "");
    sbfLog_setLevel (log, SBF_LOG_OFF);

    while ((opt = getopt (argc, argv, "h:i:t:v:")) != -1) {
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

    properties = sbfKeyValue_create ();
    sbfKeyValue_put (properties, "transport.default.type", handler);
    sbfKeyValue_put (properties, "transport.default.udp.interface", interf);

    mw = sbfMw_create (log, properties);

    queue = sbfQueue_create (mw, "default");
    sbfThread_create (&t, dispatchCb, queue);

    tport = sbfTport_create (mw, "default", SBF_MW_ALL_THREADS);

    pub = sbfRequestPub_create (tport, queue, topic, NULL, NULL);

    sbfTimer_create (sbfMw_getDefaultThread (mw),
                     queue,
                     timerCb,
                     pub,
                     1);

    for (;;)
        sleep (3600);

    exit (0);
}
