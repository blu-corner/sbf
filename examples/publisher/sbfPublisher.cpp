/*!
   \file sbfPublisher.cpp
   \brief This file defines a publisher sending messages for a given rate.
   \Copyright 2014-2018 Neueda Ltd.
*/

// External dependencies
#include <time.h>
#include <memory>

// SBF dependencies
#include "sbfCommon.h"
#include "sbfPerfCounter.h"
#include "SbfPub.hpp"
#include "SbfTimer.hpp"
#include "SbfTimer.hpp"

//----------------------------------------------------------------------------------------------------------------------
// Globals
//----------------------------------------------------------------------------------------------------------------------

uint32_t          gPublished;
neueda::SbfQueue* queue;

//----------------------------------------------------------------------------------------------------------------------
// Consts
//----------------------------------------------------------------------------------------------------------------------

#define MILLISECONDS_TO_NANO 1000000

//----------------------------------------------------------------------------------------------------------------------
// Private functions
//----------------------------------------------------------------------------------------------------------------------

/*!
   \brief This is a callback raised by the thread consuming messages from the queue.
   \param closure the data linked to the timer callback.
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
                 public neueda::SbfPubDelegate
{
public:
    virtual ~Delegate() { };
    virtual void onTicked ()
    {
        printf ("%u" SBF_EOL, gPublished);
        fflush (stdout);
        // Example stops after publishing 1000 to test memory leaks
        if (gPublished >= 1000)
        {
            delete queue;
        }
    }

    virtual void onPubReady (neueda::SbfPub* pub)
    {
        printf ("Pub ready" SBF_EOL);
        fflush (stdout);
    }
};

/*!
   \brief This function prints out the usage of the publisher command.
   \param argv0 the name of the command.
*/
static void
usage (const char* argv0)
{
    fprintf (stderr,
             "usage: %s "
             "[-h handler] "
             "[-i interface] "
             "[-r rate] "
             "[-s size] "
             "[-t topic] "
             "[-v level]\n",
             argv0);

    exit (1);
}

/*!
 * \brief Sends payload a given number or times
 * \param pub the publisher handler.
 * \param payload pointer to the payload data.
 * \param size the size of the payload
 * \param rate the time in nanoseconds between every send operation.
 * \param amount the amount to publish
 */
static void sendPerfCounterX (neueda::SbfPub* pub, 
                              uint64_t *payload, 
                              size_t size, 
                              uint64_t rate,
                              uint64_t amount)
{
    uint64_t interval;
    uint64_t until;

    // Translate timing into ticks (high performance counter)
    if (rate > 0)
        interval = (uint64_t)(sbfPerfCounter_ticks (1000000) / rate);
    else
        interval = 0;

    while (gPublished < amount)
    {
        *payload = sbfPerfCounter_get ();
        until = (*payload) + interval;
        
        pub->send (payload, size);
        gPublished++;
        
        while (sbfPerfCounter_get () < until)
        {
        }
        /* nothing */;
        usleep(10000);
    }
}

//----------------------------------------------------------------------------------------------------------------------
// Public functions
//----------------------------------------------------------------------------------------------------------------------

/*!
   \brief This function is the main entry point for the subscriber command.
   You can specify the following parameters to run the subscriber:
   "[-h handler] " the type of connection handler: tcp or udp.
   "[-i interface] " the connection interface e.g. eth0.
   "[-r rate] " the rate to send a message into the topic in milliseconds.
   "[-s size] " the size of the message in bytes.
   "[-t topic] " the name of the topic where messages are published.
   \param argc The number of arguments passed to the command.
   \param argv array of null terminated strings.
   \return Error status: 0 for Successfull another indicates an error code.
*/
int
main (int argc, char** argv)
{
    const char*          argv0 = argv[0];
    sbfThread            t;
    neueda::SbfLog*      log;
    neueda::SbfKeyValue* properties;
    neueda::SbfTport*    tport;
    neueda::SbfMw*       mw;
    neueda::SbfTimer*    timer;
    neueda::SbfPub*      pub;
    int                  opt;
    const char*          topic = "OUT";
    uint64_t             rate = 1000;
    const char*          handler = "udp";
    const char*          interf = "eth0";
    unsigned long long   ull;
    uint64_t*            payload;
    size_t               size = 200;

    // Initialise the logging system
    log = new neueda::SbfLog ();
    log->setLevel (SBF_LOG_OFF);

    // Apply command options and print message if wrong option is found
    while ((opt = getopt (argc, argv, "h:i:r:s:t:v:")) != -1) {
        switch (opt) {
        case 'h':
            handler = optarg;
            break;
        case 'i':
            interf = optarg;
            break;
        case 'r':
            rate = strtoull (optarg, NULL, 10);
            break;
        case 's':
            ull = strtoull (optarg, NULL, 10);
            if (ull < sizeof t)
                ull = sizeof t;
            if (ull < SIZE_MAX)
                size = (size_t)ull;
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
    properties  = new neueda::SbfKeyValue ();
    properties->put ("transport.default.type", handler);
    properties->put ("transport.default.udp.interface", interf);

    mw = new neueda::SbfMw (log, properties);

    // Create a queue, the connection port and a thread to dispatch events
    queue = new neueda::SbfQueue (mw, "default");

    tport = new neueda::SbfTport (mw, "default", SBF_MW_ALL_THREADS);

    // Create the timer consuming messages from the queue and
    Delegate* delegate = new Delegate();
    timer = new neueda::SbfTimer (mw->getDefaultThread (), queue, delegate, 1);

    pub = new neueda::SbfPub (tport, queue, topic, delegate);

    // Alloc size for the payload and send the high performance counter values
    // during the defined rate.
    payload = static_cast<uint64_t*>(xmalloc (size));

    sbfThread_create (&t, dispatchCb, queue);
    // If perf counter is supported send message with perf counter implementation,
    // otherwise proceed with usleep.
    sendPerfCounterX (pub, payload, size, rate, 1000);

    pthread_join(t, NULL);

    // Technically these are never hit but showing how to tidy up the classes
    delete pub;
    delete timer;
    delete delegate;
    delete tport;
    delete mw;
    delete properties;
    delete log;

    exit (0);
}
