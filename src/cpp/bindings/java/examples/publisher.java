/*
 * Copyright 2014-2018 Neueda Ltd.
 */
import com.neueda.sbf.*;
import java.nio.charset.StandardCharsets;
import java.io.*;

class PubDelegate extends SbfPubDelegate
{
    public void onPubReady(SbfPub pub)
    {
        System.out.println ("Pub Ready");
    }
}

class PubTimerDelegate extends SbfTimerDelegate
{
    public void onTicked()
    {
        System.out.format ("Published '%d'\n", publisher.published);
    }
}

class PublisherThread extends Thread {
    public void run() {
        System.out.println("Hello from dispatch thread!");
        publisher.queue.dispatch();
        System.out.format ("Publishing finished, published '%d'\n",
                           publisher.published - 1);
    }
}

public class publisher
{
    public static SbfQueue queue;
    public static int published;
    
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

        SbfMw mw = new SbfMw (log, properties);

        queue = new SbfQueue (mw, "default");

        SbfTport tport = new SbfTport (mw, "default", sbfJNI.SBF_MW_ALL_THREADS_get());

        PubTimerDelegate t_delegate = new PubTimerDelegate();
        PubDelegate p_delegate = new PubDelegate();

        SbfTimer timer = new SbfTimer (mw.getDefaultThread (), queue, t_delegate, 1);

        SbfPub pub = new SbfPub(tport, queue, "OUT", p_delegate);

        PublisherThread t = new PublisherThread();
        t.start();

        // Set buffer to string
        try {
            for (published = 1; published <= 1000; published++)
            {
                byte [] bytes = serialize(published);
                SbfBuffer buf = new SbfBuffer(bytes);
                pub.sendBuffer(buf);
                Thread.sleep(10);
            }
        } catch (Exception e) {
            System.out.println(e);
        }

        queue.destroy();
        t.join();
    }
}
