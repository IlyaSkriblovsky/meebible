package handtest;

import java.io.*;
import javax.microedition.io.*;
import javax.microedition.lcdui.*;

public class MyCanvas extends Canvas {
    
    int yOffset = 0;
    
    String content;

    public MyCanvas() {
        try {
            content = download();
        } catch (IOException ex) {
            ex.printStackTrace();
        }
    }
    
    
    
    protected void paint(Graphics g) {
        g.setColor(0xe0, 0xe0, 0xe0);
        g.fillRect(0, 0, getWidth(), getHeight());
        
        g.setColor(0x00, 0x00, 0xff);
        
        int face = Font.FACE_SYSTEM;
        int size = Font.SIZE_LARGE;
        
        Font normFont = Font.getFont(face, Font.STYLE_PLAIN, size);
        Font boldFont = Font.getFont(face, Font.STYLE_BOLD, size);
        Font italFont = Font.getFont(face, Font.STYLE_ITALIC, size);
        g.setFont(normFont);
        
        String text = content; // "Lorem привет dolor sit amet, consectetur adipiscing elit. Mauris tempor arcu a metus rhoncus eu rutrum purus cursus. Sed sit amet dui vitae lacus hendrerit porta ac ut neque. Donec consequat rutrum turpis, ac iaculis purus lobortis at. Maecenas mattis tincidunt tortor scelerisque suscipit. Pellentesque in mi lorem, euismod vestibulum mi. Nulla sed nisl quam, id ultricies turpis. Sed orci tellus, tristique at porta eget, euismod id tellus. Praesent vestibulum sagittis enim non facilisis. Nam dolor magna, sollicitudin a aliquam in, porttitor eu turpis. Nam vitae libero pretium nisi pulvinar suscipit vel quis magna. Mauris venenatis vestibulum elit id congue. Etiam pellentesque porttitor tempus. Cras condimentum porttitor neque, at bibendum urna cursus sit amet. Aenean eu velit ligula, sit amet bibendum neque. Nulla eget lacus vel arcu porttitor egestas quis nec lectus. Quisque lobortis dapibus nibh nec scelerisque. Etiam semper, velit faucibus dignissim convallis, sem massa mattis felis, non egestas lorem turpis sed augue. Sed vestibulum luctus turpis eu rutrum. Phasellus sit amet erat augue. Donec vestibulum lacus eu elit euismod interdum vehicula justo elementum.";
        char[] chars = text.toCharArray();
        int first = 0;
        int last = text.indexOf(' ');
        
        int x = 0;
        int y = -yOffset;
        int spaceWidth = g.getFont().charWidth(' ');
        int fontHeight = g.getFont().getHeight();
        while (first < text.length()) {
            int wordWidth = g.getFont().charsWidth(chars, first, last - first);
            
            if (x + wordWidth > getWidth()) {
                x = 0;
                y += g.getFont().getHeight();
                
                if (y > getHeight())
                    break;
            }
                 
            if (y + fontHeight > 0)
                g.drawChars(chars, first, last - first, x, y, 0);
            
            x += wordWidth;
            x += spaceWidth;
            
            
            first = last + 1;
            last  = text.indexOf(' ', first + 1);
        }
    }
    
    
    
    protected void handleKey(int keyCode) {
        int action = getGameAction(keyCode);
        switch (action) {
            case DOWN:
                yOffset += 10;
                repaint();
                break;
                
            case UP:
                yOffset -= 10;
                repaint();
                break;
        }
    }

    
    
    protected void keyPressed(int keyCode) {
        handleKey(keyCode);
    }
    protected void keyRepeated(int keyCode) {
        handleKey(keyCode);
    }
    
    
    
    private String download() throws IOException {
        HttpConnection httpConn = null;
        String url = "http://meebible.org/chapter?lang=u&trans=rus&book=mt&chapter=23";
        InputStream is = null;
        OutputStream os = null;
        
        String result = "<invalid>";
        
        try {
            httpConn = (HttpConnection)Connector.open(url);
            
            httpConn.setRequestMethod(HttpConnection.GET);
            httpConn.setRequestProperty("User-Agent", "MIDP test");
            
            System.out.println("Hello, World!");
            
            int respCode = httpConn.getResponseCode();
            if (respCode == HttpConnection.HTTP_OK) {
                StringBuffer sb = new StringBuffer();
                os = httpConn.openOutputStream();
                is = httpConn.openDataInputStream();
                InputStreamReader isr = new InputStreamReader(is, "UTF-8");
                
                int chr;
                while ((chr = isr.read()) != -1)
                    sb.append((char)chr);
                
                result = sb.toString();
            }
            else {
                System.out.println("Error opening HTTP Connection: " + respCode);
            }
        }
        finally {
            if (is != null) is.close();
            if (os != null) os.close();
            if (httpConn != null) httpConn.close();
        }
        
        return result;
    }
}
