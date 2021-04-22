/*!
   \file sbfSubscriber.cpp
   \brief TBD
   \Copyright 2014-2018 Neueda Ltd.
*/
#include "sbfCommon.h"
#include "sbfPerfCounter.h"
#include "SbfSub.hpp"
#include "SbfTimer.hpp"

static u_int    gMessages = 1;
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
    fflush (stdout);
    neueda::SbfQueue* instance = static_cast<neueda::SbfQueue*>(closure);
    instance->dispatch();
    return NULL;
}

class Delegate : public neueda::SbfTimerDelegate,
                 public neueda::SbfSubDelegate
{
public:
    virtual ~Delegate() { };
    virtual void onTicked ()
    {
        printf ("%u low=%llu average=%llu high=%llu" SBF_EOL,
        gMessages,
        gTimeLow == UINT64_MAX ? 0 : (unsigned long long)gTimeLow,
        gMessages == 0 ? 0 : (unsigned long long)gTimeTotal / gMessages,
        (unsigned long long)gTimeHigh);
        fflush (stdout);

        gTimeLow = UINT64_MAX;
        gTimeTotal = 0;
        gTimeHigh = 0;
        if (gMessages >= 1000)
        {
            pthread_exit(NULL);
        }
    }

    virtual void onSubReady (neueda::SbfSub* sub)
    {
        printf ("Sub ready" SBF_EOL);
        fflush (stdout);
    }
    
    virtual void onSubMessage (neueda::SbfSub* sub, neueda::SbfBuffer* buffer)
    {
        uint64_t* payload = static_cast<uint64_t*> (buffer->getData ());
        uint64_t  now;
        double    interval = -1;

        if(CAP_HI_RES_COUNTER == sbfMw_checkSupported(CAP_HI_RES_COUNTER))
        {
            now = sbfPerfCounter_get ();
            if (now > *payload)
                interval = sbfPerfCounter_microseconds (now - *payload);
            gTimeTotal += (uint64_t)interval;
            if (interval < gTimeLow)
                gTimeLow = (uint64_t)interval;
            if (interval > gTimeHigh)
                gTimeHigh = (uint64_t)interval;
        }

        gMessages++;
    }
};

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
    const char* argv0 =  argv[0];
    neueda::SbfMw*       mw;
    neueda::SbfQueue*    queue;
    sbfThread            t;
    neueda::SbfTport*    tport;
    neueda::SbfKeyValue* properties;
    neueda::SbfLog*      log;
    neueda::SbfSub*      sub;
    neueda::SbfTimer*    timer;
    int                  opt;
    const char*          topic = "OUT";
    const char*          handler = "udp";
    const char*          interf = "eth0";

    // Initialise the logging system
    log = new neueda::SbfLog ();
    log->setLevel (SBF_LOG_OFF);

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
            log->setLevelFromString (optarg);
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
    properties = new neueda::SbfKeyValue ();
    properties->put ("transport.default.type", handler);
    properties->put ("transport.default.udp.interface", interf);
    properties->put ("transport.default.tcpmesh.listen", "0");
    properties->put ("transport.default.tcpmesh.connect0", "127.0.0.1");

    mw = new neueda::SbfMw (log, properties);

    // Create a queue, the connection port and a thread to dispatch events
    queue = new neueda::SbfQueue (mw, "default");
    sbfThread_create (&t, dispatchCb, queue);

    tport = new neueda::SbfTport (mw, "default", SBF_MW_ALL_THREADS);

    // Create the timer consuming messages from the queue and
    Delegate* delegate = new Delegate();
    timer = new neueda::SbfTimer (mw->getDefaultThread (), queue, delegate, 1);

    // Create the subscriber
    sub = new neueda::SbfSub (tport, queue, topic, delegate);

    pthread_join(t, NULL);
    delete sub;
    delete timer;
    delete delegate;
    delete tport;
    delete queue;
    delete mw;
    delete properties;
    delete log;
    exit (0);
}
