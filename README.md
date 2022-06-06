# PROJECT MOVED TO [Version1 Org/sbf](https://github.com/Version1/sbf). THIS REPOSITORY WILL NO LONGER BE MAINTAINED.

# SBF (Super Blazing Fast) a messaging orientated middleware in C. [![Build Status](https://travis-ci.com/blu-corner/sbf.svg?branch=master)](https://travis-ci.com/blu-corner/sbf) [![Quality Gate Status](https://sonarcloud.io/api/project_badges/measure?project=blu-corner_sbf&metric=alert_status)](https://sonarcloud.io/dashboard?id=blu-corner_sbf)

SBF gives publish/subscribe and request/reply messaging semantics in addition to core networking libraries and common utils (queues, timers and event handling) to build applications. The library uses async callback idiom to deliver messages/events/timers etc. The core event engine is provided by the open source library libevent.

For messaging a choice of TCP or UDP multicast can be used depending on the use case of the application. UDP multicast is better if there is a one to many publish/subscribe topology.

For publish/subscribe a topic is created and one or many publishers can publish messages on the topic and one or many subscribers can listen for messages on that topic.

For request/reply, a publisher sends a single request on a topic and one or more subscribers can send a reply to that specific request for the specific publisher.

SBF is brokerless and uses a deterministic addressing scheme for topic resolution.

## Getting Started

To compile the installation for Linux:

```bash
$ git submodule update --init --recursive
$ mkdir build
$ cd build
$ cmake ../
$ make install

```

### Dependencies

All dependencies are managed through git submodules.

### API modules

* Core
  * [Event](./src/core/sbfEvent.h)
  * Timers
    * [Timer](./src/core/sbfTimer.h)
    * [Hi resolution timer](./src/core/sbfHiResTimer.h)
  * [Queue](./src/core/sbfQueue.h)
  * [Middleware](./src/core/sbfMw.h)
* Handlers
  * [Common handler](./src/handlers/common/sbfCommonHandler.h)
  * [TCP mesh handler](./src/handlers/tcpmesh/sbfTcpMeshHandler.h)
  * [UDP handler](./src/handlers/udp/sbfUdpHandler.h)
* Transport, pub/sub and Request/reply
  * [Transport](./src/transport/sbfTport.h)
  * [Publisher](./src/transport/sbfPub.h)
  * [Subscriber](./src/transport/sbfSub.h)
  * [Request publisher](./src/requestreply/sbfRequestPub.h)
  * [Request subscriber](./src/requestreply/sbfRequestSub.h)
  * [Request reply](./src/requestreply/sbfRequestReply.h)

### Example Usage

Examples for publish/subscribe, request/reply can be found in [examples](./examples) directory this is the best place to start.

When creating an application, typically you need to create the following base compoonents

* Log
* KeyValue for properties
* MW
* Transport
* Queue 
* Thead to dispatch Queue

Once complete, publishers, subscribers, timers and events can be created, the callbacks will be delivered via the dispatching queue

The following shows creating each component

#### Create Log

```c
// Initialise the logging system
log = sbfLog_create (NULL, "%s", "");
sbfLog_setLevel (log, SBF_LOG_OFF);
```

#### Create configuration 

```c
// Initialise the middleware by defining some properties like the handler
// (e.g. tcp, udp) and the  connection interface (e.g. eth0).
properties = sbfKeyValue_create ();
sbfKeyValue_put (properties, "transport.default.type", "udp");
sbfKeyValue_put (properties, "transport.default.udp.interface", "eth0");
```

#### Create MW

```c
mw = sbfMw_create (log, properties);
```

#### Create Transport

```c
tport = sbfTport_create (mw, "default", SBF_MW_ALL_THREADS);
```

#### Create Queue and thread to dispatch

```c
// Thread entry point for queue dispatch
static void*
dispatchCb (void* closure)
{
    sbfQueue_dispatch (closure);
    return NULL;
}

// Create a queue, the connection port and a thread to dispatch events
queue = sbfQueue_create (mw, "default");
sbfThread_create (&t, dispatchCb, queue);
```

#### Create Subscriber for topic

```c
// message callback function
static void
messageCb (sbfSub sub, sbfBuffer buffer, void* closure)
{
    uint64_t* payload = sbfBuffer_getData (buffer);
    uint64_t  now;
    uint64_t  this = *payload;
    double    interval = 0;

    if(CAP_HI_RES_COUNTER == sbfMw_checkSupported (CAP_HI_RES_COUNTER))
    {
        now = sbfPerfCounter_get ();
        if (now > this)
            interval = sbfPerfCounter_microseconds (now - this);
        gTimeTotal += (uint64_t)interval;
        if (interval < gTimeLow)
            gTimeLow = (uint64_t)interval;
        if (interval > gTimeHigh)
            gTimeHigh = (uint64_t)interval;
    }

    gMessages++;
}

sbfSub_create (tport, queue, "TEST_TOPIC", NULL, messageCb, NULL);
```

