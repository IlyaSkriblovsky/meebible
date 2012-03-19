package meebible;

import javax.microedition.lcdui.Graphics;

public class ChapterRenderer {
    
    class Paragraph {
        final String markup;
        
        Paragraph(String markup) {
            this.markup = markup;
        }
        
        
        private int height = -1;
        int getHeight() {
            if (height == -1)
                height = parRenderer.parHeight(markup);
            
            return height;
        }
        
        void resetHeight() {
            height = -1;
        }
    }


    ParagraphRenderer parRenderer;
    RenderCanvas canvas;
    
    Paragraph[] pars;
    int anchorParNo = 0;
    
    ChapterRenderer(RenderCanvas canvas, String text) {
        this.canvas = canvas;
        parRenderer = new ParagraphRenderer(canvas);
        
        long t1 = System.currentTimeMillis();
        
        int parts = 1;
        int delim = text.indexOf('|');
        while (delim != -1) {
            parts++;
            delim = text.indexOf('|', delim+1);
        }
        
        pars = new Paragraph[parts];
        int start = 0;
        int i = 0;
        delim = text.indexOf('|');
        while (delim != -1) {
            pars[i++] = new Paragraph(text.substring(start, delim));
            start = delim+1;
            delim = text.indexOf('|', delim+1);
        }
        
        pars[i] = new Paragraph(text.substring(start, text.length()));
    }
    
    
    void setAnchorParNo(int parNo) { anchorParNo = parNo; }
    int getAnchorParNo() { return anchorParNo; }
    
    
    public void stupidDraw(Graphics g, int yOffset, int y, int h) {
//        for (int i = 0; i < pars.length; i++) {
//            if (yOffset + pars[i].getHeight() > y)
//                parRenderer.drawPar(pars[i].markup, g, yOffset, y, h);
//
//            yOffset += pars[i].getHeight();
//            
//            if (yOffset > y+h)
//                break;
//        }
        
        int parNo;
        int curY = 0;

        parNo = anchorParNo;
        curY = yOffset;
        while (parNo < pars.length) {
            if (curY > y + h) break;
            
            if (curY + pars[parNo].getHeight() > y)
                parRenderer.drawPar(pars[parNo].markup, g, curY, y, h);
            
            curY += pars[parNo].getHeight();
            parNo++;
        }
        
        parNo = anchorParNo - 1;
        if (parNo >= 0)
            curY = yOffset - pars[parNo].getHeight();
        while (parNo >= 0) {
            if (curY + pars[parNo].getHeight() < y) break;
            
            if (curY < y + h)
                parRenderer.drawPar(pars[parNo].markup, g, curY, y, h);
            
            parNo--;
            if (parNo >= 0)
                curY -= pars[parNo].getHeight();
        }
    }
    
    
//    private int chapterHeight = -1;
//    public int getChapterHeight() {
//        if (chapterHeight == -1) {
//            long t1 = System.currentTimeMillis();
//            chapterHeight = 0;
//            for (int i = 0; i < pars.length; i++)
//                chapterHeight += pars[i].getHeight();
//        }
//        return chapterHeight;
//    }
    
    
    public void resetLayout() {
        for (int i = 0; i < pars.length; i++)
            pars[i].resetHeight();
        // chapterHeight = -1;
    }
    
    
    static final class VerseLocation {
        public final int paragraphNo;
        public final int verseOffset;
        
        public VerseLocation(int paragraphNo, int verseOffset) {
            this.paragraphNo = paragraphNo;
            this.verseOffset = verseOffset;
        }
        
        public String toString() {
            return "VerseLocation(p=" + paragraphNo + ", y=" + verseOffset + ")";
        }
    }
    
    VerseLocation findVerse(int verseNo) {
        Paragraph p = null;
                
        String needle = "{" + verseNo + "}";
        int i;
        for (i = 0; i < pars.length; i++)
            // This will fail for escaped braces, for example \{2}
            if (pars[i].markup.indexOf(needle) != -1) {
                p = pars[i];
                break;
            }
        
        if (p == null) return new VerseLocation(-1, 0);
        
        return new VerseLocation(i, parRenderer.verseOffset(p.markup, verseNo));
    }
}
