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
    int         opt;
    const char* topic = "OUT";
    const char* handler = "udp";
    const char* interf = "eth0";

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
            sbfLog_setLevel (sbfLog_levelFromString (optarg));
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

    sbfRequestSub_create (tport, queue, topic, NULL, requestCb, NULL);

    for (;;)
        sleep (3600);

    exit (0);
}
