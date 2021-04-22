/*
 * Copyright 2014-2018 Neueda Ltd.
 */
import com.neueda.sbf.*;
import java.nio.charset.StandardCharsets;
import java.io.*;

class SubDelegate extends SbfSubDelegate
{
    public void onSubReady(SbfSub sub)
    {
        System.out.println ("Sub Ready");
    }

    public void onSubMessage(SbfSub sub, SbfBuffer buffer)
    {
        try {
            int i = (int)subscriber.deserialize(buffer.getByteArray());
            System.out.format("Got int '%d'\n", i);
            if (i == 20)
            {
                subscriber.queue.destroy();
                System.out.format ("Subscribing finished\n");
            }
        } catch (Exception e) {
            System.out.println(e);
        }
    }
}

class SubscriberThread extends Thread {
    public void run() {
        System.out.println("Hello from dispatch thread!");
        subscriber.queue.dispatch();
    }
}

public class subscriber
{
    public static SbfQueue queue;

    public static byte[] serialize(Object obj) throws IOException {
        ByteArrayOutputStream out = new ByteArrayOutputStream();
        ObjectOutputStream os = new ObjectOutputStream(out);
        os.writeObject(obj);
        return out.toByteArray();
    }
    public static Object deserialize(byte[] data) throws IOException, ClassNotFoundException {
        ByteArrayInputStream in = new ByteArrayInputStream(data);
        ObjectInputStream is = new ObjectInputStream(in);
        Object ret = is.readObject();
        return ret;
    }

    public static void main(String[] args) throws InterruptedException
    {
        SbfLog log = new SbfLog ();
        log.setLevel (sbfLogLevel.SBF_LOG_OFF);

        SbfKeyValue properties = new SbfKeyValue();
        properties.put ("transport.default.type", "udp");
        properties.put ("transport.default.udp.interface", "eth0");
        properties.put ("transport.default.tcpmesh.listen", "0");
        properties.put ("transport.default.tcpmesh.connect0", "127.0.0.1");

        SbfMw mw = new SbfMw (log, properties);

        queue = new SbfQueue (mw, "default");

        SbfTport tport = new SbfTport (mw, "default", sbfJNI.SBF_MW_ALL_THREADS_get());

        SubDelegate s_delegate = new SubDelegate();

        SbfSub sub = new SbfSub(tport, queue, "OUT", s_delegate);

        SubscriberThread t = new SubscriberThread();
        t.start();
        t.join();
    }
}
