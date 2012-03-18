package rendertest;

import java.io.*;
import java.util.Enumeration;
import java.util.Vector;
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
    
    String langCode;
    String transCode;
    Book[] books;

    Command cmdPlace = new Command("Open place", Command.SCREEN, 1);
    Command cmdFontSize = new Command("Font size", Command.SCREEN, 2);
    Command cmdLangTrans = new Command("Lang & Trans", Command.SCREEN, 3);
    Command cmdDebugPage = new Command("Debug", Command.SCREEN, 4);
    Command cmdExit = new Command("Exit", Command.EXIT, 5);
    
    RenderCanvas(RenderMidlet midlet) {
        this.midlet = midlet;
    
        chapRenderer = new ChapterRenderer(this, "");
        
        setFontSize(FontSize.MEDIUM);
        
        setCommandListener(this);
        
        addCommand(cmdPlace);
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
    public final void setFontSize(int fontSize) {
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
                public void selected(String langCode, String transCode, Book[] books) {
                    RenderCanvas.this.langCode = langCode;
                    RenderCanvas.this.transCode = transCode;
                    RenderCanvas.this.books = books;
                    midlet.show(RenderCanvas.this);
                }
                
                public void cancelled() {
                    midlet.show(RenderCanvas.this);
                }
            });
        }
        else if (command == cmdPlace) {
            midlet.show(new PlaceSelector(books, new PlaceSelector.Listener() {
                public void selected(Book book, int chapterNo, int verseNo) {
                    System.out.println("SELECTED: " + book.code + " " + chapterNo + ":" + verseNo);
                    loadChapter(book, chapterNo, verseNo);
                }

                public void cancelled() { midlet.show(RenderCanvas.this); }
            }));
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
    
    
    
    public void loadChapter(Book book, int chapterNo, int verseNo) {
        String url = "http://meebible.org/chapter.j2me?lang=" + langCode +
                "&trans=" + transCode + "&book=" + book.code +
                "&chapter=" + chapterNo;
        System.out.println(url);
        
        final LoadingSplash splash = new LoadingSplash("Loading chapter...");
        
        Loader.load(url, new LoadListener() {
            public void finished(String content) {
                chapRenderer = new ChapterRenderer(RenderCanvas.this, content);
                yOffset = 0;
                repaint();
                splash.close(RenderCanvas.this);
            }

            public void error() {
                splash.close(RenderCanvas.this);
                throw new UnsupportedOperationException("Not supported yet.");
            }
        });
    }
}
