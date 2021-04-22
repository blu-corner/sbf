from sbf import *
import threading
import time
import pickle
import sys
import traceback

def dispatch(queue):
    queue.dispatch()


class PublisherDelegate(SbfPubDelegate):
    def onPubReady(self, pub):
        print("Publisher Ready")

class TimerDelegate(SbfTimerDelegate):
    def onTicked(self):
        global published, queue
        print("Published ", published)
        if (published == 1000):
            print("Destroying")
            queue.destroy()

log = SbfLog ()
log.setLevel (SBF_LOG_DEBUG)
log.log (SBF_LOG_INFO, "Publisher example started")

properties = SbfKeyValue()
properties.put ("transport.default.type", "udp")
properties.put ("transport.default.udp.interface", "eth0")

mw = SbfMw (log, properties)

queue = SbfQueue (mw, "default")

tport = SbfTport (mw, "default", SBF_MW_ALL_THREADS)

t_delegate = TimerDelegate()
p_delegate = PublisherDelegate()

timer = SbfTimer (mw.getDefaultThread (), queue, t_delegate, 1)

pub = SbfPub(tport, queue, "OUT", p_delegate)

t = threading.Thread(target=dispatch, args=(queue,))
t.start()

for published in range(1001):
    payload = pickle.dumps (published)
    pub.send(bytearray(payload))
    time.sleep(0.01)

t.join()
del pub
del timer
del tport
del queue
del mw
del properties
del log
