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
