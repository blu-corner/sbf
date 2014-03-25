#include "sbfCommon.h"
#include "sbfMw.h"
#include "sbfRequestReply.h"
#include "sbfTport.h"

static void*
dispatchCb (void* closure)
{
    sbfQueue_dispatch (closure);
    return NULL;
}

static void
replyCb (sbfRequestPub pub, sbfRequest req, sbfBuffer buffer, void* closure)
{
    uint64_t* value;

    if (sbfBuffer_getSize (buffer) < sizeof *value)
        return;
    value = sbfBuffer_getData (buffer);
    printf ("<-- reply %llu\n", (unsigned long long)*value);
}

static void
timerCb (sbfTimer timer, void* closure)
{
    static uint64_t next;

    next++;
    sbfRequestPub_send (closure, &next, sizeof next, replyCb, NULL);
    printf ("--> request %llu\n", (unsigned long long)next);
}

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
    const char*   argv0 = argv[0];
    sbfMw         mw;
    sbfQueue      queue;
    sbfThread     t;
    sbfTport      tport;
    sbfKeyValue   properties;
    sbfRequestPub pub;
    int           opt;
    const char*   topic = "OUT";
    const char*   handler = "udp";
    const char*   interf = "eth0";

    sbfLog_setLevel (SBF_LOG_OFF);

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
            sbfLog_setLevel (sbfLog_getLevel (optarg));
            break;
        default:
            usage (argv0);
        }
    }
    argc -= optind;
    argv += optind;
    if (argc != 0)
        usage (argv0);

    mw = sbfMw_create (1);

    queue = sbfQueue_create (0);
    sbfThread_create (&t, dispatchCb, queue);

    properties = sbfKeyValue_create ();
    sbfKeyValue_put (properties, "interface", interf);

    tport = sbfTport_create (mw, SBF_MW_ALL_THREADS, handler, properties);

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
