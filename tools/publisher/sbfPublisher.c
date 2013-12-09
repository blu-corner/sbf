#include "sbfCommon.h"
#include "sbfCpuFreq.h"
#include "sbfMw.h"
#include "sbfTport.h"

uint32_t gPublished;

static void*
dispatchCb (void* closure)
{
    sbfQueue_dispatch (closure);
    return NULL;
}

static void
timerCb (sbfTimer timer, void* closure)
{
    printf ("%u\n", gPublished);

    gPublished = 0;
}

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

int
main (int argc, char** argv)
{
    const char* argv0 = argv[0];
    sbfMw       mw;
    sbfQueue    queue;
    pthread_t   t;
    sbfTport    tport;
    sbfKeyValue properties;
    sbfPub      pub;
    uint64_t    interval;
    uint64_t    until;
    int         opt;
    const char* topic = "OUT";
    uint64_t    rate = 1000;
    const char* handler = "udp";
    const char* interface = "eth0";
    size_t      size = 200;
    uint64_t*   payload;

    sbfLog_setLevel (SBF_LOG_OFF);

    while ((opt = getopt (argc, argv, "h:i:r:s:t:v:")) != -1) {
        switch (opt) {
        case 'h':
            handler = optarg;
            break;
        case 'i':
            interface = optarg;
            break;
        case 'r':
            rate = strtoull (optarg, NULL, 10);
            break;
        case 's':
            size = strtoull (optarg, NULL, 10);
            if (size < sizeof t)
                size = sizeof t;
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
    pthread_create (&t, NULL, dispatchCb, queue);

    properties = sbfKeyValue_create ();
    sbfKeyValue_put (properties, "interface", interface);

    tport = sbfTport_create (mw, SBF_MW_ALL_THREADS, handler, properties);

    sbfTimer_create (sbfMw_getDefaultThread (mw),
                     queue,
                     timerCb,
                     NULL,
                     1);

    pub = sbfPub_create (tport, queue, topic, NULL, NULL);

    if (rate > 0)
        interval = (sbfCpuFreq_get () * 1000 * 1000) / rate;
    else
        interval = 0;

    payload = xmalloc (size);
    for (;;)
    {
        *payload = sbfRdtsc ();
        until = (*payload) + interval;

        sbfPub_send (pub, payload, size);
        gPublished++;

        while (sbfRdtsc () < until)
            /* nothing */;
    }

    exit (0);
}
