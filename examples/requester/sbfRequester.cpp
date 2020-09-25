/*!
   \file sbfRequester.cpp
   \brief TBD
   \Copyright 2014-2018 Neueda Ltd.
*/
#include "SbfMw.hpp"
#include "SbfRequestPub.hpp"
#include "SbfTport.hpp"
#include "SbfTimer.hpp"

neueda::SbfRequestPub* reqPub;
neueda::SbfQueue*      queue;

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
                 public neueda::SbfRequestPubDelegate
{
public:
    virtual ~Delegate() { };
    virtual void onTicked ()
    {
        static uint64_t next;

        next++;
        reqPub->send (&next, sizeof next);
        printf ("--> request %llu sent\n", (unsigned long long)next);
    }

    virtual void onPubReady (neueda::SbfRequestPub* pub)
    {
        printf ("Requester Ready\n");
    }

    virtual void onPubReply (neueda::SbfRequestPub* pub,
                             struct sbfRequestImpl* req,
                             neueda::SbfBuffer* buf)
    {
        uint64_t* value;

        if (buf->getSize () < sizeof *value)
            return;
        value = static_cast<uint64_t*>(buf->getData ());
        printf ("<-- reply %llu received\n", (unsigned long long)*value);
        if (*value == 20)
        {
            printf ("Requester Ending\n");
            delete queue;
        }
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
    neueda::SbfMw*       mw;
    neueda::SbfTport*    tport;
    neueda::SbfKeyValue* properties;
    neueda::SbfLog*      log;
    neueda::SbfTimer*    timer;
    sbfThread            t;
    int           opt;
    const char*   topic = "OUT";
    const char*   handler = "udp";
    const char*   interf = "eth0";

    log = new neueda::SbfLog ();
    log->setLevel (SBF_LOG_OFF);

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

    properties = new neueda::SbfKeyValue ();
    properties->put ("transport.default.type", handler);
    properties->put ("transport.default.udp.interface", interf);

    mw = new neueda::SbfMw (log, properties);

    queue = new neueda::SbfQueue (mw, "default");
    sbfThread_create (&t, dispatchCb, queue);

    tport = new neueda::SbfTport (mw, "default", SBF_MW_ALL_THREADS);

    Delegate* delegate = new Delegate();
    reqPub = new neueda::SbfRequestPub (tport, queue, topic, delegate);

    timer = new neueda::SbfTimer (mw->getDefaultThread (), queue, delegate, 1);

    sbfThread_join (t);
    delete timer;
    delete reqPub;
    delete tport;
    delete mw;
    delete properties;
    delete log;
    delete delegate;

    exit (0);
}
