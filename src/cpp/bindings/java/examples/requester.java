/*
 * Copyright 2014-2018 Neueda Ltd.
 */
import com.neueda.sbf.*;
import java.nio.charset.StandardCharsets;
import java.io.*;

class RequesterDelegate extends SbfRequestPubDelegate
{
    public void onPubReady(SbfRequestPub pub)
    {
        System.out.println ("Request Pub Ready");
    }

    public void onPubReply(SbfRequestPub pub,
                           SWIGTYPE_p_sbfRequestImpl req,
                           SbfBuffer buff)
    {
        try {
            int i = (int) requester.deserialize(buff.getData ());
            System.out.format("Received reply '%d'\n", i);
            if (i == 20)
            {
                requester.queue.destroy();
                System.out.println ("Request Pub Exiting");
            }
        } catch (Exception e) {
        }
    }
}

class RequesterTimerDelegate extends SbfTimerDelegate
{
    public void onTicked()
    {
        try {
            System.out.format("Sending request '%d'\n", requester.published);
            byte [] bytes = requester.serialize(requester.published++);
            requester.pub.send(bytes);
        } catch (Exception e) {
        }
    }
}

class RequesterThread extends Thread {
    public void run() {
        System.out.println("Hello from dispatch thread!");
        requester.queue.dispatch();
    }
}

public class requester
{
    public static SbfQueue queue;
    public static SbfRequestPub pub;
    public static int published = 1;

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

        RequesterTimerDelegate t_delegate = new RequesterTimerDelegate();
        RequesterDelegate r_delegate = new RequesterDelegate();

        pub = new SbfRequestPub(tport, queue, "OUT", r_delegate);

        SbfTimer timer = new SbfTimer (mw.getDefaultThread (), queue, t_delegate, 1);

        RequesterThread t = new RequesterThread();
        t.start();
        t.join();
    }
}
