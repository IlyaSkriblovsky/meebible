package rendertest;

import java.io.*;
import javax.microedition.lcdui.*;

public class RenderCanvas extends Canvas implements CommandListener {
    
    public class FontSize {
        public static final int SMALL = 10;
        public static final int MEDIUM = 20;
        public static final int LARGE = 30;
    }
    

    
    
    RenderMidlet midlet;
    
    ChapterRenderer chapRenderer;
    
    int yOffset = 0;
    
    Command cmdFontSize = new Command("Font size", Command.SCREEN, 1);
    Command cmdLangTrans = new Command("Lang & Trans", Command.SCREEN, 2);
    Command cmdDebugPage = new Command("Debug", Command.SCREEN, 3);
    Command cmdExit = new Command("Exit", Command.EXIT, 4);
    
    RenderCanvas(RenderMidlet midlet) {
        this.midlet = midlet;
    
        chapRenderer = new ChapterRenderer(this, loadChapter());
        
        setFontSize(FontSize.MEDIUM);
        
        setCommandListener(this);
        
        addCommand(cmdFontSize);
        addCommand(cmdDebugPage);
        addCommand(cmdLangTrans);
        addCommand(cmdExit);
    }
    
    
    String loadChapter() {
        try {
            InputStream is = getClass().getResourceAsStream("chapter.txt");
            Reader decoder = new InputStreamReader(is, "UTF-8");
            StringBuffer buf = new StringBuffer();
            int ch;
        
            char[] chs = new char[4096];
            int len;
            while ((len = decoder.read(chs, 0, chs.length)) != -1)
                buf.append(chs, 0, len);
            
            return buf.toString();
        }
        catch (Exception e) {
            e.printStackTrace();
            return "cannot load";
        }
    }
    
    
    private Font font;
    
    
    private int fontSize = FontSize.MEDIUM;
    public int getFontSize() { return fontSize; }
    public void setFontSize(int fontSize) {
        this.fontSize = fontSize;
        
        int lcduiFontSize = Font.SIZE_MEDIUM;
        switch (fontSize) {
            case FontSize.SMALL: lcduiFontSize = Font.SIZE_SMALL; break;
            case FontSize.LARGE: lcduiFontSize = Font.SIZE_LARGE; break;
        }
        font = Font.getFont(Font.FACE_SYSTEM, Font.STYLE_PLAIN, lcduiFontSize);
        
        chapRenderer.onFontSizeChanged();
        setYOffset(yOffset);
    }
    public Font getFont() { return font; }
    
    
    protected void paint(Graphics g) {
        g.setColor(0xe0, 0xe0, 0xe0);
        g.fillRect(0, 0, getWidth(), getHeight());
        
        g.setFont(getFont());

        chapRenderer.stupidDraw(g, - yOffset);
    }
    
    
    void setYOffset(int y) {
        yOffset = y;
        
        if (yOffset < 0) this.yOffset = 0;
//        if (yOffset + getHeight() > chapRenderer.getChapterHeight())
//            yOffset = chapRenderer.getChapterHeight() - getHeight();
        
        repaint();
    }
    void scrollBy(int offset) {
        setYOffset(yOffset + offset);
    }
    
    private void handleKey(int keyCode) {
        switch (getGameAction(keyCode)) {
            case UP: {
                scrollBy(- getFont().getHeight());
                break;
            }
                
            case DOWN: {
                scrollBy(+ getFont().getHeight());
                break;
            }
            
            case LEFT: {
                scrollBy(- getHeight());
                break;
            }
                
            case RIGHT: {
                scrollBy(+ getHeight());
                break;
            }
        }
    }
    
    protected void keyPressed(int keyCode) { handleKey(keyCode); }
    protected void keyRepeated(int keyCode) { handleKey(keyCode); }
    
    
    public void commandAction(Command command, Displayable displayable) {
        if (command == cmdFontSize)
            midlet.showFontSelector();
        else if (command == cmdDebugPage)
            midlet.showDebugPage();
        else if (command == cmdExit)
            midlet.destroyApp(false);
        else if (command == cmdLangTrans) {
            midlet.getLangTransDialog().open(new LangTransDialog.Listener() {
                public void selected(String langCode, String transCode) {
                    System.out.println("langCode=" + langCode + " transCode=" + transCode);
                    midlet.show(RenderCanvas.this);
                }
                
                public void cancelled() {
                    midlet.show(RenderCanvas.this);
                }
            });
        }
    }
    
    
    
    int dragOrig;
    protected void pointerPressed(int x, int y) {
        dragOrig = y;
    }
    
    protected void pointerDragged(int x, int y) {
        scrollBy(dragOrig - y);
        dragOrig = y;
    }
}
