# SBF (Super Blazing Fast) a messaging orientated middleware in C.

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
To compile the installation for Windows, same as above but change cmake command for nmake

```bash
> mkdir build
> cd build
> cmake -DCMAKE_WINDOWS_EXPORT_ALL_SYMBOLS=TRUE -G"NMake Makefiles" ../
> nmak
```

### Dependencies

All dependencies are managed through git submodules.

### API modules

* Core
  * [Event](@ref sbfEvent.h)
  * Timers
    * [Timer](./core/sbfTimer.h)
    * [Hi resolution timer](./core/sbfHiResTimer.h)
  * [Queue](./core/sbfQueue.h)
  * [Middleware](./core/sbfMw.h)
* Handlers
  * [Common handler](@ref sbfCommonHandler.h)
  * [TCP mesh handler](@ref sbfTcpMeshHandler.h)
  * [UDP handler](@ref sbfUdpHandler.h)
* Transport, pub/sub and Request/reply
  * [Transport](@ref sbfTport.h)
  * [Publisher](@ref sbfPub.h)
  * [Subscriber](@ref sbfSub.h)
  * [Request publisher](@ref sbfRequestPub.h)
  * [Request subscriber](@ref sbfRequestSub.h)
  * [Request reply](@ref sbfRequestReply.h)

### Example Usage

Examples for publish/subscribe, request/reply can be found in ./tools/ directory this is the best place to start

When creating application, typically you need to create the following base compoonents

* Log
* KeyValue for properties
* MW
* Transport
* Queue 
* Thead to dispatch Queue

Once complete, publishers, subscriber, timers and events can be created, the callbacks will be delivered via the dispatching queue

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

## Vagrant
If you are running on OS X or Windows we provide a vagrant file to create a VirtualBox ubuntu image including all the necessary stuff to run.
You will need to download [VirtualBox](https://www.virtualbox.org/) and [Vagrant](https://www.vagrantup.com/).
1. Unzip ubuntu.zip folder
`user@host:~$ unzip ubuntu.zip`
2. Change your directory to ubuntu.
`user@host:~$ cd ubuntu`
3. Edit the `synced_folders` file and replace the `hostpath` pointing to your shared folder (this will mount the directory under /mnt/host/).
4. Run vagrant
`user@host:~/ubuntu$ vagrant up`
5. Now you are ready to run the Ubuntu image on VirtualBox
6. You will be asked to prompt your username and password
`username:vagrant`
`password:vagrant`

