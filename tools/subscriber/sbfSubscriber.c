#include "sbfCommon.h"
#include "sbfCpuFreq.h"
#include "sbfMw.h"
#include "sbfTport.h"

static u_int    gMessages;
static uint64_t gTimeLow = UINT64_MAX;
static uint64_t gTimeTotal;
static uint64_t gTimeHigh;

static void*
dispatchCb (void* closure)
{
    sbfQueue_dispatch (closure);
    return NULL;
}

static void
timerCb (sbfTimer timer, void* closure)
{
    printf ("%u low=%llu average=%llu high=%llu\n",
            gMessages,
            gTimeLow == UINT64_MAX ? 0 : (unsigned long long)gTimeLow,
            gMessages == 0 ? 0 : (unsigned long long)gTimeTotal / gMessages,
            (unsigned long long)gTimeHigh);

    gMessages = 0;
    gTimeLow = UINT64_MAX;
    gTimeTotal = 0;
    gTimeHigh = 0;
}

static void
messageCb (sbfSub sub, sbfBuffer buffer, void* closure)
{
    uint64_t* payload = sbfBuffer_getData (buffer);
    uint64_t  now;
    uint64_t  this = *payload;
    uint64_t  interval;

    now = sbfRdtsc ();
    if (now < this)
        return;
    interval = (now - this) / sbfCpuFreq_get ();

    gMessages++;
    gTimeTotal += interval;
    if (interval < gTimeLow)
        gTimeLow = interval;
    if (interval > gTimeHigh)
        gTimeHigh = interval;
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
    pthread_t   t;
    sbfTport    tport;
    sbfKeyValue properties;
    int         opt;
    const char* topic = "OUT";
    const char* handler = "udp";
    const char* interface = "eth0";

    sbfLog_setLevel (SBF_LOG_OFF);

    while ((opt = getopt (argc, argv, "h:i:t:v:")) != -1) {
        switch (opt) {
        case 'h':
            handler = optarg;
            break;
        case 'i':
            interface = optarg;
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

    mw = sbfMw_create (5);

    queue = sbfQueue_create (0);
    pthread_create (&t, NULL, dispatchCb, queue);

    properties = sbfKeyValue_create ();
    sbfKeyValue_put (properties, "interface", interface);

    sbfKeyValue_put (properties, "weight0", "OUT__1,100");

    tport = sbfTport_create (mw, SBF_MW_ALL_THREADS, handler, properties);

    sbfTimer_create (sbfMw_getDefaultThread (mw),
                     queue,
                     timerCb,
                     NULL,
                     1);

    int i;
    sbfSub s[10];
    for (i = 0; i < 10; i++)
    {
        char topic0[100];
        snprintf (topic0, sizeof topic0, "%s__%d", topic, i);
        s[i] = sbfSub_create (tport, queue, topic0, NULL, messageCb, NULL);
    }

    sleep (3);

    sbfTport_destroy (tport);

    exit (0);
}
