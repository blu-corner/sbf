/*!
   \file sbfReplier.c
   \brief TBD
   \Copyright 2014-2018 Neueda Ltd.
*/
#include "SbfMw.hpp"
#include "SbfRequestSub.hpp"
#include "SbfTport.hpp"

neueda::SbfQueue*    queue;

/*!
   \brief Callback from thread to dispatch the items in the queue.
   \param closure data linked to the thread callback.
*/
static void*
dispatchCb (void* closure)
{
    fflush (stdout);
    neueda::SbfQueue* instance = static_cast<neueda::SbfQueue*>(closure);
    instance->dispatch();
    return NULL;
}

/*!
   \brief Callback that replies with the number of request performed.
   \param sub  pointer to subscriber.
   \param req pointer to request.
   \param buffer data being requested.
   \param closure the data linked to this callback.
*/
class Delegate : public neueda::SbfRequestSubDelegate
{
public:
    virtual void onSubReady (neueda::SbfRequestSub* sub)
    {
        printf ("Replier Ready\n");
    }

    virtual void onSubRequest (neueda::SbfRequestSub* sub,
                               struct sbfRequestImpl* req,
                               neueda::SbfBuffer* buf)
    {
        uint64_t*       value;

        if (buf->getSize () < sizeof *value)
            return;
        value = static_cast<uint64_t*>(buf->getData ());
        printf ("<-- request %llu received\n", (unsigned long long)*value);

        sub->reply (req, value, sizeof *value);
        printf ("--> reply %llu sent\n", (unsigned long long)*value);
        if (*value == 20)
        {
            printf ("Replier Ending\n");
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
   \brief This function is the main entry point for the replier command.
   You can specify the following parameters to run the replier:
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
    neueda::SbfMw*       mw;
    neueda::SbfTport*    tport;
    neueda::SbfKeyValue* properties;
    neueda::SbfLog*      log;
    sbfThread            t;
    int                  opt;
    const char*          topic = "OUT";
    const char*          handler = "udp";
    const char*          interf = "eth0";

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
    neueda::SbfRequestSub* reqSub = new neueda::SbfRequestSub (tport, queue, topic, delegate);

    sbfThread_join (t);
    delete reqSub;
    delete tport;
    delete mw;
    delete properties;
    delete log;
    delete delegate;

    exit (0);
}
