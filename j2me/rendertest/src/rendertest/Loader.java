package rendertest;

import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import javax.microedition.io.Connector;
import javax.microedition.io.HttpConnection;
import javax.microedition.io.StreamConnection;

public class Loader {
    
    static final int BUFFER_SIZE = 2048;
    
    static class Worker extends Thread {
        String url;
        LoadListener listener;
        
        Worker(String url, LoadListener listener) {
            this.url = url;
            this.listener = listener;
        }
        
        public void run() {
            try {
                HttpConnection conn = (HttpConnection)Connector.open(url);
                InputStream is = conn.openInputStream();
                InputStreamReader r = new InputStreamReader(is, "UTF-8");
                StringBuffer sb = new StringBuffer();
                
                long len = conn.getLength();
                
                listener.progress(0, len);
                
                char[] buf = new char[BUFFER_SIZE];
                int chunkLen;
                long total = 0;
                while ((chunkLen = r.read(buf, 0, buf.length)) != -1) {
                    sb.append(buf, 0, chunkLen);
                    total += chunkLen;
                    listener.progress(total, len);
                }

                conn.close();

                listener.finished(sb.toString());
            }
            catch (IOException e) {
                listener.error();
            }
        }
    }
        
    static void load(String url, LoadListener listener) {
        new Worker(url, listener).start();
    }
    
}
