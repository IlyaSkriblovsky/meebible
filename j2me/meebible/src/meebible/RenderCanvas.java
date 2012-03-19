package meebible;

import java.io.*;
import java.util.Enumeration;
import java.util.Hashtable;
import java.util.Vector;
import javax.microedition.lcdui.*;
import javax.microedition.lcdui.game.Sprite;
import javax.microedition.rms.RecordStore;
import javax.microedition.rms.RecordStoreException;
import javax.microedition.rms.RecordStoreNotFoundException;
import javax.microedition.rms.RecordStoreNotOpenException;

public class RenderCanvas extends Canvas implements CommandListener {
    
    MeeBibleMidlet midlet;
    
    ChapterRenderer chapRenderer;
    
    int yOffset = 0;
    
    final Book[] hardcodedBooks = { new Book("ge", "Genesis", 50), new Book("ex", "Exodus", 40), new Book("le", "Leviticus", 27), new Book("nu", "Numbers", 36), new Book("de", "Deuteronomy", 34), new Book("jos", "Joshua", 24), new Book("jg", "Judges", 21), new Book("ru", "Ruth", 4), new Book("1sa", "1 Samuel", 31), new Book("2sa", "2 Samuel", 24), new Book("1ki", "1 Kings", 22), new Book("2ki", "2 Kings", 25), new Book("1ch", "1 Chronicles", 29), new Book("2ch", "2 Chronicles", 36), new Book("ezr", "Ezra", 10), new Book("ne", "Nehemiah", 13), new Book("es", "Esther", 10), new Book("job", "Job", 42), new Book("ps", "Psalms", 150), new Book("pr", "Proverbs", 31), new Book("ec", "Ecclesiastes", 12), new Book("ca", "Song of Solomon", 8), new Book("isa", "Isaiah", 66), new Book("jer", "Jeremiah", 52), new Book("la", "Lamentations", 5), new Book("eze", "Ezekiel", 48), new Book("da", "Daniel", 12), new Book("ho", "Hosea", 14), new Book("joe", "Joel", 3), new Book("am", "Amos", 9), new Book("ob", "Obadiah", 1), new Book("jon", "Jonah", 4), new Book("mic", "Micah", 7), new Book("na", "Nahum", 3), new Book("hab", "Habakkuk", 3), new Book("zep", "Zephaniah", 3), new Book("hag", "Haggai", 2), new Book("zec", "Zechariah", 14), new Book("mal", "Malachi", 4), new Book("mt", "Matthew", 28), new Book("mr", "Mark", 16), new Book("lu", "Luke", 24), new Book("joh", "John", 21), new Book("ac", "Acts", 28), new Book("ro", "Romans", 16), new Book("1co", "1 Corinthians", 16), new Book("2co", "2 Corinthians", 13), new Book("ga", "Galatians", 6), new Book("eph", "Ephesians", 6), new Book("php", "Philippians", 4), new Book("col", "Colossians", 4), new Book("1th", "1 Thessalonians", 5), new Book("2th", "2 Thessalonians", 3), new Book("1ti", "1 Timothy", 6), new Book("2ti", "2 Timothy", 4), new Book("tit", "Titus", 3), new Book("phm", "Philemon", 1), new Book("heb", "Hebrews", 13), new Book("jas", "James", 5), new Book("1pe", "1 Peter", 5), new Book("2pe", "2 Peter", 3), new Book("1jo", "1 John", 5), new Book("2jo", "2 John", 1), new Book("3jo", "3 John", 1), new Book("jude", "Jude", 1), new Book("re", "Revelation", 22) };

    String langCode = "e";
    String transCode = "nwt";
    Book[] books = hardcodedBooks;
    int bookNo = 0;
    int chapterNo = 1;
    String content = "";
    
    Image offscreen;
    Graphics og;

    Command cmdNextChapter = new Command("Next", Command.SCREEN, 1);
    Command cmdPlace = new Command("Open place", Command.SCREEN, 2);
    Command cmdFontSize = new Command("Font size", Command.SCREEN, 3);
    Command cmdLangTrans = new Command("Lang & Trans", Command.SCREEN, 4);
    Command cmdExit = new Command("Exit", Command.EXIT, 5);
    
    RenderCanvas(MeeBibleMidlet midlet) {
        this.midlet = midlet;
        
        createOffscreen();
    
        chapRenderer = new ChapterRenderer(this, "");
        
        setCommandListener(this);
        
        addCommand(cmdNextChapter);
        addCommand(cmdPlace);
        addCommand(cmdFontSize);
        addCommand(cmdLangTrans);
        addCommand(cmdExit);
        
        loadSettings();
        
        setFullScreenMode(fullscreen);
        
        if (content.length() > 0)
            showContent(content, yOffset);
        else
            loadChapter(bookNo, chapterNo, 1, yOffset);
    }

    private void createOffscreen() {
        offscreen = Image.createImage(getWidth(), getHeight());
        og = offscreen.getGraphics();
    }
    
    protected void sizeChanged(int w, int h) {
        createOffscreen();
        forceRepaint();
    }

    private boolean fullscreen = false;
    protected void showNotify() {
        setFullScreenMode(fullscreen);
    }
    
    

    
    
    private int fontSize = Font.SIZE_MEDIUM;
    public int getFontSize() { return fontSize; }
    public final void setFontSize(int fontSize) {
        this.fontSize = fontSize;

        resetFonts();
        
        chapRenderer.onFontSizeChanged();
        forceRepaint();
    }
    

    private Hashtable fonts = new Hashtable();
    private void resetFonts() {
        fonts.clear();
    }
    public Font getFont() {
        return getFont(Font.STYLE_PLAIN);
    }
    public Font getFont(int style) {
        if (fonts.contains(new Integer(style)))
            return (Font)fonts.get(new Integer(style));
        
        Font font = Font.getFont(Font.FACE_SYSTEM, style, fontSize);
        fonts.put(new Integer(style), font);
        return font;
    }
    
    
    
    
    private int lastDrawnYOffset = -999;
    private boolean forceRepaint = true;
    protected void paint(Graphics g) {
        if (getHeight() != offscreen.getHeight()) {
            // Workaround for a bug in NetBeans 7.1.1 simulator
            createOffscreen();
            forceRepaint = true;
        }
            
        
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
                
            case FIRE: {
                fullscreen = !fullscreen;
                setFullScreenMode(fullscreen);
                break;
            }
        }
    }
    
    protected void keyPressed(int keyCode) { handleKey(keyCode); }
    protected void keyRepeated(int keyCode) { handleKey(keyCode); }
    
    
    public void commandAction(Command c, Displayable d) {
        if (c == cmdFontSize)
            midlet.showFontSelector();
        else if (c == cmdExit)
            midlet.destroyApp(false);
        else if (c == cmdLangTrans) {
            midlet.getLangTransDialog().open(new LangTransDialog.Listener() {
                public void selected(String langCode, String transCode, Book[] books) {
                    RenderCanvas.this.langCode = langCode;
                    RenderCanvas.this.transCode = transCode;
                    RenderCanvas.this.books = books;
                    midlet.show(RenderCanvas.this);
                    loadChapter(bookNo, chapterNo, 1);
                }
                
                public void cancelled() {
                    midlet.show(RenderCanvas.this);
                }
            });
        }
        else if (c == cmdPlace) {
            midlet.show(new PlaceSelector(books, bookNo, chapterNo, new PlaceSelector.Listener() {
                public void selected(int bookNo, int chapterNo, int verseNo) {
                    loadChapter(bookNo, chapterNo, verseNo);
                }

                public void cancelled() { midlet.show(RenderCanvas.this); }
            }));
        }
        else if (c == cmdNextChapter)
            nextChapter();
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
    
    
    public final void loadChapter(int bookNo, int chapterNo, int verseNo) {
        loadChapter(bookNo, chapterNo, verseNo, 0);
    }
    public final void loadChapter(final int bookNo, final int chapterNo, int verseNo, final int initialYOffset) {
        this.chapterNo = chapterNo;
        this.bookNo = bookNo;

        String url = "http://meebible.org/chapter.j2me?lang=" + langCode +
                "&trans=" + transCode + "&book=" + books[bookNo].code +
                "&chapter=" + chapterNo;
        System.out.println(url);
        
        final LoadingSplash splash = new LoadingSplash("Loading chapter...");
        
        Loader.load(url, new LoadListener() {
            public void finished(String content) {
                String title = "^B^U" + books[bookNo].name + " " + chapterNo + "^b^u|";
                RenderCanvas.this.showContent(title + content, initialYOffset);
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
        try {
            settings = RecordStore.openRecordStore("settings", false);
            ByteArrayInputStream bis = new ByteArrayInputStream(settings.getRecord(1));
            DataInputStream dis = new DataInputStream(bis);            

            langCode = dis.readUTF();
            transCode = dis.readUTF();
            bookNo = dis.readInt();
            chapterNo = dis.readInt();
            fontSize = dis.readInt();
            yOffset = dis.readInt();
            fullscreen = dis.readBoolean();
        }
        catch (Exception e) {
            e.printStackTrace(); 
            langCode = "e";
            transCode = "nwt";
            bookNo = 0;
            chapterNo = 1;
            fontSize = Font.SIZE_MEDIUM;
            yOffset = 0;
        }
        finally {
            try { if (settings != null) settings.closeRecordStore(); }
            catch (RecordStoreException e) { }
        }


        RecordStore lastChapter = null;
        try {
            lastChapter = RecordStore.openRecordStore("lastChapter", false);
            content = new String(lastChapter.getRecord(1));
        }
        catch (Exception e) { e.printStackTrace(); content = ""; }
        finally {
            try { if (lastChapter != null) lastChapter.closeRecordStore(); }
            catch (RecordStoreException e) { }
        }

        
        RecordStore bookList = null;
        try {
            bookList = RecordStore.openRecordStore("bookList", false);
            books = new Book[bookList.getNumRecords()];
            for (int i = 0; i < books.length; i++) {
                ByteArrayInputStream bis = new ByteArrayInputStream(bookList.getRecord(i+1));
                DataInputStream dis = new DataInputStream(bis);
                String code = dis.readUTF();
                String name = dis.readUTF();
                int chapterCount = dis.readInt();
                books[i] = new Book(code, name, chapterCount);
            }
        }
        catch (Exception e) {
            e.printStackTrace(); 
            books = hardcodedBooks;
        }
        finally {
            try { if (bookList != null) bookList.closeRecordStore(); }
            catch (RecordStoreException e) { }
        }
    }
    
    
    public void saveSettings() {
        try {
            RecordStore.deleteRecordStore("settings");
            RecordStore.deleteRecordStore("lastChapter");
            RecordStore.deleteRecordStore("bookList");
        }
        catch (RecordStoreException e) { }

        RecordStore settings = null;
        try {
            settings = RecordStore.openRecordStore("settings", true);
            ByteArrayOutputStream bos = new ByteArrayOutputStream();
            DataOutputStream dos = new DataOutputStream(bos);
            dos.writeUTF(langCode);
            dos.writeUTF(transCode);
            dos.writeInt(bookNo);
            dos.writeInt(chapterNo);
            dos.writeInt(fontSize);
            dos.writeInt(yOffset);
            dos.writeBoolean(fullscreen);
            dos.close();
            byte[] bytes = bos.toByteArray();
            settings.addRecord(bytes, 0, bytes.length);
        }
        catch (Exception e) { e.printStackTrace(); }
        finally {
            try { if (settings != null) settings.closeRecordStore(); }
            catch (RecordStoreException e) { }
        }

        RecordStore lastChapter = null;
        try {
            lastChapter = RecordStore.openRecordStore("lastChapter", true);
            byte[] b_content = content.getBytes();
            lastChapter.addRecord(b_content, 0, b_content.length);
        }
        catch (Exception e) { e.printStackTrace(); }
        finally {
            try { if (lastChapter != null) lastChapter.closeRecordStore(); }
            catch (RecordStoreException e) { }
        }


        RecordStore bookList = null;
        try {
            bookList = RecordStore.openRecordStore("bookList", true);
            for (int i = 0; i < books.length; i++) {
                ByteArrayOutputStream bos = new ByteArrayOutputStream();
                DataOutputStream dos = new DataOutputStream(bos);
                dos.writeUTF(books[i].code);
                dos.writeUTF(books[i].name);
                dos.writeInt(books[i].chapterCount);
                dos.close();
                byte[] bytes = bos.toByteArray();
                bookList.addRecord(bytes, 0, bytes.length);
            }
        }
        catch (Exception e) { e.printStackTrace(); }
        finally {
            try { if (bookList != null) bookList.closeRecordStore(); }
            catch (RecordStoreException e) { }
        }
    }
    
    
    
    void nextChapter() {
        if (chapterNo < books[bookNo].chapterCount)
            loadChapter(bookNo, chapterNo + 1, 1);
        else
            if (bookNo +1 < books.length)
                loadChapter(bookNo+1, 1, 1);
    }
}
