from sbf import *
import threading
import time
import struct
import ctypes

def dispatch(queue):
    queue.dispatch()

class RequesterDelegate(SbfRequestPubDelegate):
    def onPubReady(self, pub):
        print("Requester Ready")

    def onPubReply(self, pub, req, buf):
        global queue
        b = bytes(bytearray(buf.getByteArray(),'utf-8'))
        x = struct.unpack('I8s',b)
        print("Received reply", x[0])
        if x[0] == 20:
            queue.destroy()


class TimerDelegate(SbfTimerDelegate):
    def onTicked(self):
        global requester, published
        print("Sending request", published)
        s = bytes('Sending', 'utf-8')
        packed_data = struct.pack('I7s', published, s)
        requester.send(bytearray(packed_data))
        published += 1

log = SbfLog ()
log.setLevel (SBF_LOG_DEBUG)
log.log (SBF_LOG_INFO, "Requester example started")

properties = SbfKeyValue()
properties.put ("transport.default.type", "udp")
properties.put ("transport.default.udp.interface", "eth0")

mw = SbfMw (log, properties)

queue = SbfQueue (mw, "default")

tport = SbfTport (mw, "default", SBF_MW_ALL_THREADS)

t_delegate = TimerDelegate()
r_delegate = RequesterDelegate()

timer = SbfTimer (mw.getDefaultThread (), queue, t_delegate, 1)

requester = SbfRequestPub(tport, queue, "OUT", r_delegate)

t = threading.Thread(target=dispatch, args=(queue,))
published = 1
t.start()
t.join()
del requester
del timer
del tport
del queue
del mw
del properties
del log