package rendertest;

import java.io.*;
import java.util.Enumeration;
import java.util.Vector;
import javax.microedition.lcdui.*;
import javax.microedition.lcdui.game.Sprite;
import javax.microedition.rms.RecordStore;
import javax.microedition.rms.RecordStoreException;
import javax.microedition.rms.RecordStoreNotFoundException;
import javax.microedition.rms.RecordStoreNotOpenException;

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
    int bookNo = 0;
    int chapterNo = 1;
    String content = "";
    
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
        
        setCommandListener(this);
        
        addCommand(cmdPlace);
        addCommand(cmdFontSize);
        addCommand(cmdLangTrans);
        addCommand(cmdExit);
        
        loadSettings();
        
        if (content.length() > 0)
            showContent(content, yOffset);
        else
            loadChapter(books[bookNo], chapterNo, 1, yOffset);
    }
    
        
    private Font font;
    
    
    private int fontSize = FontSize.MEDIUM;
    public int getFontSize() { return fontSize; }
    public final void setFontSize(int fontSize) {
        this.fontSize = fontSize;

        font = null;
        
        chapRenderer.onFontSizeChanged();
        forceRepaint();
    }
    public Font getFont() {
        if (font == null) {
            int lcduiFontSize = Font.SIZE_MEDIUM;
            switch (fontSize) {
                case FontSize.SMALL: lcduiFontSize = Font.SIZE_SMALL; break;
                case FontSize.LARGE: lcduiFontSize = Font.SIZE_LARGE; break;
            }
            font = Font.getFont(Font.FACE_SYSTEM, Font.STYLE_PLAIN, lcduiFontSize);
        }
        return font;
    }
    
    
    
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
    
    
    
    public final void showContent(String content, int initialYOffset) {
        this.content = content;
        chapRenderer = new ChapterRenderer(this, content);
        yOffset = initialYOffset;
        forceRepaint();
    }
    
    
    public final void loadChapter(Book book, int chapterNo, int verseNo) {
        loadChapter(book, chapterNo, verseNo, 0);
    }
    public final void loadChapter(Book book, int chapterNo, int verseNo, final int initialYOffset) {
        this.chapterNo = chapterNo;
        bookNo = 0;
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
                RenderCanvas.this.showContent(content, initialYOffset);
                splash.close(RenderCanvas.this);
            }

            public void error() {
                splash.close(RenderCanvas.this);
                throw new UnsupportedOperationException("Not supported yet.");
            }
        });
    }
    
    
    
    public final void loadSettings() {
        RecordStore settings = null;
        RecordStore lastChapter = null;
        try {
            settings = RecordStore.openRecordStore("settings", false);
            
            langCode = new String(settings.getRecord(1));
            transCode = new String(settings.getRecord(2));
            bookNo = settings.getRecord(3)[0];
            chapterNo = settings.getRecord(4)[0];
            fontSize = settings.getRecord(5)[0];
            yOffset = Integer.parseInt(new String(settings.getRecord(6)));

            
            lastChapter = RecordStore.openRecordStore("lastChapter", false);
            try { content = new String(lastChapter.getRecord(1)); }
            catch (Exception e) { content = ""; }
        }
        catch (RecordStoreException e) {
            e.printStackTrace();
        }
        finally {
            try {
                if (settings != null) settings.closeRecordStore();
                if (lastChapter != null) lastChapter.closeRecordStore();
            } catch (RecordStoreException ex) {
                ex.printStackTrace();
            }
        }
    }
    
    
    public void saveSettings() {
        RecordStore settings = null;
        RecordStore lastChapter = null;
        try {
            try {
                RecordStore.deleteRecordStore("settings");
                RecordStore.deleteRecordStore("lastChapter");
            }
            catch (RecordStoreNotFoundException e) { }
            
            settings = RecordStore.openRecordStore("settings", true);
            
            // 1
            byte[] b_langCode = langCode.getBytes();
            settings.addRecord(b_langCode, 0, b_langCode.length);
            
            // 2
            byte[] b_transCode = transCode.getBytes();
            settings.addRecord(b_transCode, 0, b_transCode.length);
            
            // 3
            byte[] b_bookNo = new byte[] { (byte)bookNo };
            settings.addRecord(b_bookNo, 0, b_bookNo.length);
            
            // 4
            byte[] b_chapterNo = new byte[] { (byte)chapterNo };
            settings.addRecord(b_chapterNo, 0, b_chapterNo.length);
            
            // 5
            byte[] b_fontSize = new byte[] { (byte)getFontSize() };
            settings.addRecord(b_fontSize, 0, b_fontSize.length);
            
            // 6
            byte[] b_yOffset = new Integer(yOffset).toString().getBytes();
            settings.addRecord(b_yOffset, 0, b_yOffset.length);
            
            
            lastChapter = RecordStore.openRecordStore("lastChapter", true);
            byte[] b_content = content.getBytes();
            lastChapter.addRecord(b_content, 0, b_content.length);
        }
        catch (RecordStoreException e) {
            e.printStackTrace();
        }
        finally {
            try {
                if (settings != null) settings.closeRecordStore();
                if (lastChapter != null) lastChapter.closeRecordStore();
            } catch (RecordStoreException ex) {
                ex.printStackTrace();
            }
        }
    }
}
