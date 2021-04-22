from sbf import *
import threading

def dispatch(queue):
    queue.dispatch()


class QueueDelegate1(SbfQueueDelegate):
    def onQueueItem(self):
        print("Queue Delegate 1")

class QueueDelegate2(SbfQueueDelegate):
    def onQueueItem(self):
        print("Queue Delegate 2")

log = SbfLog ()
log.setLevel (SBF_LOG_DEBUG)
log.log (SBF_LOG_INFO, "Queue example started")

properties = SbfKeyValue()
mw = SbfMw (log, properties)

queue = SbfQueue (mw, "default")
delegate1 = QueueDelegate1()
delegate2 = QueueDelegate2()
queue.enqueue (delegate1)
queue.enqueue (delegate1)
queue.enqueue (delegate2)
queue.enqueue (delegate1)
queue.enqueue (delegate2)
queue.enqueue (delegate2)
queue.enqueue (delegate1)

t = threading.Thread(target=dispatch, args=(queue,))
t.start()
queue.destroy()
t.join()
del queue
del mw
del properties
del log