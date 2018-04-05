# SBF - Super Bloody Fast
SBF is a message transport system that allows inter app communication in two different flavours:  
* Publisher - Subscriber
* Requester - Replier

The `publisher-subscriber` consists of publishers defining topics where messages are placed. Then one or more subscribers might register to a concrete topic to read the messages that were delivered (one to many communication).  
On the other hand, the `requester-replier` communication is about defining a channel where messages are sent from one requester to one publisher (one to one communication).

## Compilation Unix

Windows can be built with make

```
$ mkdir build
$ cd build
$ cmake ../
$ make
$ make install
```

## Compilation Windows

Windows can be built with cmake

```
> mkdir build
> cd build
> cmake -DCMAKE_WINDOWS_EXPORT_ALL_SYMBOLS=TRUE -G"NMake Makefiles" ../
> nmake
```

> TBD

## API
Here you are the list of modules the API offers to build your transport
* Core
  * [Event](@ref sbfEvent.h)
  * Timers
    * [Timer](@ref sbfTimer.h)
    * [Hi resolution timer](@ref sbfHiResTimer.h)
  * [Queue](@ref sbfQueue.h)
  * [Middleware](@ref sbfMw.h)
* Handlers
  * [Common handler](@ref sbfCommonHandler.h)
  * [TCP mesh handler](@ref sbfTcpMeshHandler.h)
  * [UDP handler](@ref sbfUdpHandler.h)
* Request / reply
  * [Request publisher](@ref sbfRequestPub.h)
  * [Request subscriber](@ref sbfRequestSub.h)
  * [Request reply](@ref sbfRequestReply.h)

## Publisher - subscriber example
Let's see how to create a publisher-subscriber communication.

### Publisher code
Here are the first lines you will need to create your publisher:

#### Create the middleware  
```c
#include "sbfCommon.h" // Common stuff such as queues, threading, ...
#include "sbfMw.h" // The middleware API

...

// Initialise the middleware by defining some properties like the handler (e.g. tcp, udp) and the  connection interface (e.g. eth0).
sbfKeyValue properties = sbfKeyValue_create ();
sbfKeyValue_put (properties, "transport.default.type", "udp");
sbfKeyValue_put (properties, "transport.default.udp.interface", "eth0");
sbfMw mw = sbfMw_create (log, properties);
```

#### Create a queue, the connection port and a thread to dispatch events
```c
static void*
dispatchCb (void* closure)
{
    sbfQueue_dispatch (closure);
    return NULL;
}

...

sbfThread t;
sbfQueue queue = sbfQueue_create (mw, "default");
sbfThread_create (&t, dispatchCb, queue);
sbfTport tport = sbfTport_create (mw, "default", SBF_MW_ALL_THREADS);
```

#### Create the publisher
```c
static void
timerCb (sbfTimer timer, void* closure)
{
    printf ("%u" SBF_EOL, gPublished);
    fflush (stdout);

    gPublished = 0;
}

...

sbfTimer_create (sbfMw_getDefaultThread (mw),
                 queue,
                 timerCb,
                 NULL,
                 1);
sbfPub pub = sbfPub_create (tport, queue, "topic", NULL, NULL);
```

You can find the whole example of the publisher  [here](https://gitlab.com/neueda/sbf/tree/master/tools/publisher)

### Subscriber code
Now let's see how the subscriber works.

#### Create the middleware  
```c
#include "sbfCommon.h" // Common stuff such as queues, threading, ...
#include "sbfMw.h" // The middleware API

...

// Initialise the middleware by defining some properties like the handler (e.g. tcp, udp) and the  connection interface (e.g. eth0).
sbfKeyValue properties = sbfKeyValue_create ();
sbfKeyValue_put (properties, "transport.default.type", "udp");
sbfKeyValue_put (properties, "transport.default.udp.interface", "eth0");
sbfKeyValue_put (properties, "transport.default.tcpmesh.listen", "0");
sbfKeyValue_put (properties, "transport.default.tcpmesh.connect0", "127.0.0.1");
sbfMw mw = sbfMw_create (log, properties);
```

#### Create a queue, the connection port and a thread to dispatch events
```c
static void*
dispatchCb (void* closure)
{
    sbfQueue_dispatch (closure);
    return NULL;
}

...

sbfThread t;
sbfQueue queue = sbfQueue_create (mw, "default");
sbfThread_create (&t, dispatchCb, queue);
sbfTport tport = sbfTport_create (mw, "default", SBF_MW_ALL_THREADS);
```

#### Create the subscriber
```c
static void
timerCb (sbfTimer timer, void* closure)
{
    printf ("%u low=%llu average=%llu high=%llu" SBF_EOL,
            gMessages,
            gTimeLow == UINT64_MAX ? 0 : (unsigned long long)gTimeLow,
            gMessages == 0 ? 0 : (unsigned long long)gTimeTotal / gMessages,
            (unsigned long long)gTimeHigh);
}

static void
messageCb (sbfSub sub, sbfBuffer buffer, void* closure)
{
    uint64_t* payload = sbfBuffer_getData (buffer);
}
...

sbfTimer_create (sbfMw_getDefaultThread (mw),
                 queue,
                 timerCb,
                 NULL,
                 1);
sbfPub pub = sbfPub_create (tport, queue, "topic", NULL, NULL);
```

You can find the whole example of the subscriber   [here](https://gitlab.com/neueda/sbf/tree/master/tools/subscriber).

## Requester replier example
> TBD

# Download
Download the source code from GitLab at https://gitlab.com/neueda/sbf.
SBT run under Linux OS (Debian, Ubuntu, Centos distros are highly recommended).

## Vagrant
If you are running on OS X or Windows don't worry, we provide a vagrant file to create a VirtualBox ubuntu image including all the necessary stuff to run.
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

# Running the examples
You will find the SBF together with some examples at GitLab (https://gitlab.com/neueda.sbf/). Thus, clone the repository:
`user@host:~$ git clone https://gitlab.com/neueda.sbf/`

## Build the SBF
Build the SBF by running the following commands:
```
cd sbf
mkdir build && cd build
cmake ..
make build && make install
```
You will find the binaries at `cd build/install/bin`.
```
sbf-replier
sbf-requester
sbf-publisher
sbf-subscriber
```
The requester sends messages and the replier reads the message and replies.
Something similar for the publisher and subscriber.

# Contribute
> TBD

# License
> TBD
