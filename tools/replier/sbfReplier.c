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

    sbfRequestSub_create (tport, queue, topic, NULL, requestCb, NULL);

    for (;;)
        sleep (3600);

    exit (0);
}
