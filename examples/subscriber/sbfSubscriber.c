/*!
   \file sbfSubscriber.c
   \brief TBD
   \Copyright 2014-2018 Neueda Ltd.
*/
#include "sbfCommon.h"
#include "sbfMw.h"
#include "sbfPerfCounter.h"
#include "sbfTport.h"

static u_int    gMessages;
static uint64_t gTimeLow = UINT64_MAX;
static uint64_t gTimeTotal;
static uint64_t gTimeHigh;

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
   \brief Timer callback to process a request.
   \param timer callback fired by timer.
   \param closure data linked to the callback.
*/
static void
timerCb (sbfTimer timer, void* closure)
{
    printf ("%u low=%llu average=%llu high=%llu" SBF_EOL,
            gMessages,
            gTimeLow == UINT64_MAX ? 0 : (unsigned long long)gTimeLow,
            gMessages == 0 ? 0 : (unsigned long long)gTimeTotal / gMessages,
            (unsigned long long)gTimeHigh);
    fflush (stdout);

    gMessages = 0;
    gTimeLow = UINT64_MAX;
    gTimeTotal = 0;
    gTimeHigh = 0;
}

/*!
   \brief Callback processing
   \param sub the subscriber handler.
   \param buffer the received buffer.
   \param closure data linked to the callback.
*/
static void
messageCb (sbfSub sub, sbfBuffer buffer, void* closure)
{
    uint64_t* payload = sbfBuffer_getData (buffer);
    uint64_t  now;
    uint64_t  this = *payload;
    double    interval = 0;

    if(CAP_HI_RES_COUNTER == sbfMw_checkSupported(CAP_HI_RES_COUNTER))
    {
        now = sbfPerfCounter_get ();
        if (now > this)
            interval = sbfPerfCounter_microseconds (now - this);
        gTimeTotal += (uint64_t)interval;
        if (interval < gTimeLow)
            gTimeLow = (uint64_t)interval;
        if (interval > gTimeHigh)
            gTimeHigh = (uint64_t)interval;
    }

    gMessages++;
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
   \brief This function is the main entry point for the subscriber command.
   You can specify the following parameters to run the subscriber:
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
    const char* argv0 = argv[0];
    sbfMw       mw;
    sbfQueue    queue;
    sbfThread   t;
    sbfTport    tport;
    sbfKeyValue properties;
    sbfLog      log;
    int         opt;
    const char* topic = "OUT";
    const char* handler = "udp";
    const char* interf = "eth0";

    // Initialise the logging system
    log = sbfLog_create (NULL, "%s", "");
    sbfLog_setLevel (log, SBF_LOG_OFF);

    // Apply command options and print message if wrong option is found
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

    // Initialise the middleware by defining some properties like the handler
    // (e.g. tcp, udp) and the  connection interface (e.g. eth0).
    properties = sbfKeyValue_create ();
    sbfKeyValue_put (properties, "transport.default.type", handler);
    sbfKeyValue_put (properties, "transport.default.udp.interface", interf);
    sbfKeyValue_put (properties, "transport.default.tcpmesh.listen", "0");
    sbfKeyValue_put (properties,
                     "transport.default.tcpmesh.connect0",
                     "127.0.0.1");

    mw = sbfMw_create (log, properties);

    // Create a queue, the connection port and a thread to dispatch events
    queue = sbfQueue_create (mw, "default");
    sbfThread_create (&t, dispatchCb, queue);

    tport = sbfTport_create (mw, "default", SBF_MW_ALL_THREADS);

    // Create the timer consuming messages from the queue and
    sbfTimer_create (sbfMw_getDefaultThread (mw),
                     queue,
                     timerCb,
                     NULL,
                     1);

    // Create the subscriber
    sbfSub_create (tport, queue, topic, NULL, messageCb, NULL);

    for (;;)
        sleep (60);

    exit (0);
}
