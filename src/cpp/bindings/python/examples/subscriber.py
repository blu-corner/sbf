from sbf import *
import threading
import time
import pickle

def dispatch(queue):
    queue.dispatch()

class SubscriberDelegate(SbfSubDelegate):
    def onSubReady(self, sub):
        print("Subscriber Ready")

    def onSubMessage(self, sub, buffer):
        global received
        received += 1
        if (received % 100 == 0):
            print("On Sub Message ", received)
        if (received == 1000):
            print("Destroying")
            queue.destroy()

log = SbfLog ()
log.setLevel (SBF_LOG_DEBUG)
log.log (SBF_LOG_INFO, "Subscriber example started")

properties = SbfKeyValue()
properties.put ("transport.default.type", "udp")
properties.put ("transport.default.udp.interface", "eth0")
properties.put ("transport.default.tcpmesh.listen", "0")
properties.put ("transport.default.tcpmesh.connect0", "127.0.0.1")

mw = SbfMw (log, properties)

queue = SbfQueue (mw, "default")

tport = SbfTport (mw, "default", SBF_MW_ALL_THREADS)

s_delegate = SubscriberDelegate()

sub = SbfSub(tport, queue, "OUT", s_delegate)

t = threading.Thread(target=dispatch, args=(queue,))
t.start()

received = 0

t.join()
del sub
del tport
del queue
del mw
del properties