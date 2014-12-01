#include "sbfCommon.h"
#include "sbfMw.h"
#include "sbfPerfCounter.h"
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
    printf ("%u" SBF_EOL, gPublished);
    fflush (stdout);

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
    const char*        argv0 = argv[0];
    sbfMw              mw;
    sbfQueue           queue;
    sbfThread          t;
    sbfTport           tport;
    sbfKeyValue        properties;
    sbfLog             log;
    sbfPub             pub;
    uint64_t           interval;
    uint64_t           until;
    int                opt;
    const char*        topic = "OUT";
    uint64_t           rate = 1000;
    const char*        handler = "udp";
    const char*        interf = "eth0";
    unsigned long long ull;
    size_t             size = 200;
    uint64_t*          payload;

    log = sbfLog_create (NULL, "%s", "");
    sbfLog_setLevel (log, SBF_LOG_OFF);

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

    sbfTimer_create (sbfMw_getDefaultThread (mw),
                     queue,
                     timerCb,
                     NULL,
                     1);

    pub = sbfPub_create (tport, queue, topic, NULL, NULL);

    if (rate > 0)
        interval = (uint64_t)(sbfPerfCounter_ticks (1000000) / rate);
    else
        interval = 0;

    payload = xmalloc (size);
    for (;;)
    {
        *payload = sbfPerfCounter_get ();
        until = (*payload) + interval;

        sbfPub_send (pub, payload, size);
        gPublished++;

        while (sbfPerfCounter_get () < until)
            /* nothing */;
    }

    exit (0);
}
