from sbf import *
import threading
import time
import struct

def dispatch(queue):
    queue.dispatch()

class ReplierDelegate(SbfRequestSubDelegate):
    def onSubReady(self, pub):
        print("Replier Ready")

    def onSubRequest(self, sub, req, buf):
        global queue
        b = bytes(bytearray(buf.getByteArray(),'utf-8'))
        x = struct.unpack('I7s',b)
        print("Received request", x[0])
        s = bytes('Replying', 'utf-8')
        packed_data = struct.pack('I8s', x[0], s)
        print("Replying request", x[0])
        sub.reply(req, bytearray(packed_data))
        if x[0] == 20:
            queue.destroy()

log = SbfLog ()
log.setLevel (SBF_LOG_DEBUG)
log.log (SBF_LOG_INFO, "Replier example started")

properties = SbfKeyValue()
properties.put ("transport.default.type", "udp")
properties.put ("transport.default.udp.interface", "eth0")

mw = SbfMw (log, properties)

queue = SbfQueue (mw, "default")

tport = SbfTport (mw, "default", SBF_MW_ALL_THREADS)

r_delegate = ReplierDelegate()

replier = SbfRequestSub(tport, queue, "OUT", r_delegate)

t = threading.Thread(target=dispatch, args=(queue,))
t.start()
t.join()
del replier
del tport
del queue
del mw
del properties
del log