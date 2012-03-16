package rendertest;

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
    
    ChapterRenderer(RenderCanvas canvas, String text) {
        this.canvas = canvas;
        parRenderer = new ParagraphRenderer(canvas);
        
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
    
    
    public void stupidDraw(Graphics g, int y) {
        for (int i = 0; i < pars.length; i++) {
            if (y + pars[i].getHeight() > 0)
                parRenderer.drawPar(pars[i].markup, g, y);
            y += pars[i].getHeight();
            
            if (y > canvas.getHeight())
                break;
        }
    }
    
    
    private int chapterHeight = -1;
    public int getChapterHeight() {
        if (chapterHeight == -1) {
            chapterHeight = 0;
            for (int i = 0; i < pars.length; i++)
                chapterHeight += pars[i].getHeight();
        }
        return chapterHeight;
    }
    
    
    public void onFontSizeChanged() {
        for (int i = 0; i < pars.length; i++)
            pars[i].resetHeight();
        chapterHeight = -1;
    }
}
