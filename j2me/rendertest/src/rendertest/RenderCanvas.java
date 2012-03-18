package rendertest;

import java.io.*;
import java.util.Enumeration;
import java.util.Vector;
import javax.microedition.lcdui.*;
import javax.microedition.lcdui.game.Sprite;

public class RenderCanvas extends Canvas implements CommandListener {
    
    public class FontSize {
        public static final int SMALL = 10;
        public static final int MEDIUM = 20;
        public static final int LARGE = 30;
    }
    

    
    
    RenderMidlet midlet;
    
    ChapterRenderer chapRenderer;
    
    int yOffset = 0;
    
    String langCode = "e";
    String transCode = "nwt";
    Book[] books = { new Book("ge", "Genesis"), new Book("ex", "Exodus"), new Book("le", "Leviticus"), new Book("nu", "Numbers"), new Book("de", "Deuteronomy"), new Book("jos", "Joshua"), new Book("jg", "Judges"), new Book("ru", "Ruth"), new Book("1sa", "1 Samuel"), new Book("2sa", "2 Samuel"), new Book("1ki", "1 Kings"), new Book("2ki", "2 Kings"), new Book("1ch", "1 Chronicles"), new Book("2ch", "2 Chronicles"), new Book("ezr", "Ezra"), new Book("ne", "Nehemiah"), new Book("es", "Esther"), new Book("job", "Job"), new Book("ps", "Psalms"), new Book("pr", "Proverbs"), new Book("ec", "Ecclesiastes"), new Book("ca", "Song of Solomon"), new Book("isa", "Isaiah"), new Book("jer", "Jeremiah"), new Book("la", "Lamentations"), new Book("eze", "Ezekiel"), new Book("da", "Daniel"), new Book("ho", "Hosea"), new Book("joe", "Joel"), new Book("am", "Amos"), new Book("ob", "Obadiah"), new Book("jon", "Jonah"), new Book("mic", "Micah"), new Book("na", "Nahum"), new Book("hab", "Habakkuk"), new Book("zep", "Zephaniah"), new Book("hag", "Haggai"), new Book("zec", "Zechariah"), new Book("mal", "Malachi"), new Book("mt", "Matthew"), new Book("mr", "Mark"), new Book("lu", "Luke"), new Book("joh", "John"), new Book("ac", "Acts"), new Book("ro", "Romans"), new Book("1co", "1 Corinthians"), new Book("2co", "2 Corinthians"), new Book("ga", "Galatians"), new Book("eph", "Ephesians"), new Book("php", "Philippians"), new Book("col", "Colossians"), new Book("1th", "1 Thessalonians"), new Book("2th", "2 Thessalonians"), new Book("1ti", "1 Timothy"), new Book("2ti", "2 Timothy"), new Book("tit", "Titus"), new Book("phm", "Philemon"), new Book("heb", "Hebrews"), new Book("jas", "James"), new Book("1pe", "1 Peter"), new Book("2pe", "2 Peter"), new Book("1jo", "1 John"), new Book("2jo", "2 John"), new Book("3jo", "3 John"), new Book("jude", "Jude"), new Book("re", "Revelation") };
    int bookNo = 1;
    int chapterNo = 1;
    
    Image offscreen;
    Graphics og;

    Command cmdPlace = new Command("Open place", Command.SCREEN, 1);
    Command cmdFontSize = new Command("Font size", Command.SCREEN, 2);
    Command cmdLangTrans = new Command("Lang & Trans", Command.SCREEN, 3);
    Command cmdExit = new Command("Exit", Command.EXIT, 5);
    
    RenderCanvas(RenderMidlet midlet) {
        this.midlet = midlet;
        
        offscreen = Image.createImage(getWidth(), getHeight());
        og = offscreen.getGraphics();
    
        chapRenderer = new ChapterRenderer(this, "");
        
        setFontSize(FontSize.MEDIUM);
        
        setCommandListener(this);
        
        addCommand(cmdPlace);
        addCommand(cmdFontSize);
        addCommand(cmdLangTrans);
        addCommand(cmdExit);
        
        loadChapter(books[0], 1, 1);
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
        forceRepaint();
    }
    public Font getFont() { return font; }
    
    
    
    private int lastDrawnYOffset = -999;
    private boolean forceRepaint = true;
    protected void paint(Graphics g) {
        og.setFont(getFont());

        if (! forceRepaint) {
            if (yOffset != lastDrawnYOffset) {
                og.setClip(0, 0, getWidth(), getHeight());
                og.copyArea(0, 0, getWidth(), getHeight(), 0, lastDrawnYOffset - yOffset, Graphics.TOP|Graphics.LEFT);

                og.setColor(0xe0e0e0);
                if (yOffset > lastDrawnYOffset) {
                    int h = yOffset - lastDrawnYOffset;
                    og.fillRect(0, getHeight() - h, getWidth(), h);
                    og.setClip(0, getHeight() - h, getWidth(), h);
                    chapRenderer.stupidDraw(og, -yOffset, getHeight() - h, h);
                }
                else {
                    int h = lastDrawnYOffset - yOffset;
                    og.fillRect(0, 0, getWidth(), h);
                    og.setClip(0, 0, getWidth(), h);
                    chapRenderer.stupidDraw(og, -yOffset, 0, h);
                }
            }
        
            g.drawRegion(
                offscreen,
                g.getClipX(), g.getClipY(), g.getClipWidth(), g.getClipHeight(),
                Sprite.TRANS_NONE,
                g.getClipX(), g.getClipY(),
                Graphics.LEFT|Graphics.TOP
            );
        }
        else {
            og.setColor(0xe0e0e0);
            og.setClip(0, 0, getWidth(), getHeight());
            og.fillRect(0, 0, getWidth(), getHeight());
            chapRenderer.stupidDraw(og, -yOffset, 0, getHeight());
            g.drawImage(offscreen, 0, 0, Graphics.LEFT|Graphics.TOP);
        }
        
        lastDrawnYOffset = yOffset;
        forceRepaint = false;
    }
    
    void forceRepaint() {
        forceRepaint = true;
        repaint();
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
        else if (command == cmdExit)
            midlet.destroyApp(false);
        else if (command == cmdLangTrans) {
            midlet.getLangTransDialog().open(new LangTransDialog.Listener() {
                public void selected(String langCode, String transCode, Book[] books) {
                    RenderCanvas.this.langCode = langCode;
                    RenderCanvas.this.transCode = transCode;
                    RenderCanvas.this.books = books;
                    midlet.show(RenderCanvas.this);
                    loadChapter(books[bookNo], chapterNo, 1);
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
        this.chapterNo = chapterNo;
        bookNo = 1;
        for (int i = 0; i < books.length; i++)
            if (books[i] == book) {
                bookNo = i;
                break;
            }
        
        String url = "http://meebible.org/chapter.j2me?lang=" + langCode +
                "&trans=" + transCode + "&book=" + book.code +
                "&chapter=" + chapterNo;
        System.out.println(url);
        
        final LoadingSplash splash = new LoadingSplash("Loading chapter...");
        
        Loader.load(url, new LoadListener() {
            public void finished(String content) {
                chapRenderer = new ChapterRenderer(RenderCanvas.this, content);
                yOffset = 0;
                forceRepaint();
                splash.close(RenderCanvas.this);
            }

            public void error() {
                splash.close(RenderCanvas.this);
                throw new UnsupportedOperationException("Not supported yet.");
            }
        });
    }
}
