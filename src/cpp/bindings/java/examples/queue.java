/*
 * Copyright 2014-2018 Neueda Ltd.
 */
import com.neueda.sbf.*;
import java.util.concurrent.TimeUnit;

class QueueDelegate1 extends SbfQueueDelegate
{
    public void onQueueItem()
    {
        System.out.println ("Queue 1");
        queue.dequeued++;
        if (queue.queued == queue.dequeued)
            queue.queue.destroy();
    }
}

class QueueDelegate2 extends SbfQueueDelegate
{
    public void onQueueItem()
    {
        System.out.println ("Queue 2");
        queue.dequeued++;
        if (queue.queued == queue.dequeued)
            queue.queue.destroy();
    }
}

class DispatchThread extends Thread {
    public void run() {
        System.out.println("Hello from dispatch thread!");
        queue.queue.dispatch();
        System.out.println("Hello from dispatch thread!");
    }
}

public class queue
{
    public static SbfQueue queue;
    public static int queued = 0;
    public static int dequeued = 0;

    public static void addToQueue(SbfQueueDelegate del)
    {
        queue.enqueue(del);
        queued++;
    }

    public static void main(String[] args) throws InterruptedException
    {
        SbfLog log = new SbfLog ();
        log.setLevel (sbfLogLevel.SBF_LOG_DEBUG);

        SbfKeyValue properties = new SbfKeyValue();
        properties.put ("transport.default.type", "udp");
        properties.put ("transport.default.udp.interface", "eth0");

        SbfMw mw = new SbfMw (log, properties);

        // Create a queue, the connection port and a thread to dispatch events
        queue = new SbfQueue (mw, "default");
        QueueDelegate1 delegate1 = new QueueDelegate1();
        QueueDelegate2 delegate2 = new QueueDelegate2();
        addToQueue (delegate1);
        addToQueue (delegate1);
        addToQueue (delegate2);
        addToQueue (delegate1);
        addToQueue (delegate2);
        addToQueue (delegate2);
        addToQueue (delegate1);

        DispatchThread t = new DispatchThread();
        t.start();
        t.join();
    }
}
