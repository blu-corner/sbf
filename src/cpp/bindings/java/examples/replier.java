/*
 * Copyright 2014-2018 Neueda Ltd.
 */
import com.neueda.sbf.*;
import java.nio.charset.StandardCharsets;
import java.io.*;

class ReplierDelegate extends SbfRequestSubDelegate
{
    public void onSubReady(SbfRequestSub sub)
    {
        System.out.println ("Request Sub Ready");
    }

    public void onSubRequest(SbfRequestSub sub,
                             SWIGTYPE_p_sbfRequestImpl req,
                             SbfBuffer buff)
    {
        try {
            int i = (int) replier.deserialize(buff.getData ());
            System.out.format ("Received request '%d'\n", i);
            System.out.format ("Sending reply '%d'\n", i);
            sub.reply (req, buff.getData ());
            if (i == 20)
            {
                replier.queue.destroy();
                System.out.println ("Request Sub Exiting");
            }
        }
        catch (Exception e)
        {
        }
    }
}

class ReplierThread extends Thread {
    public void run() {
        System.out.println("Hello from dispatch thread!");
        replier.queue.dispatch();
    }
}

public class replier
{
    public static SbfQueue queue;
    public static SbfRequestSub sub;
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

        ReplierDelegate r_delegate = new ReplierDelegate();

        sub = new SbfRequestSub(tport, queue, "OUT", r_delegate);

        ReplierThread t = new ReplierThread();
        t.start();
        t.join();
    }
}
