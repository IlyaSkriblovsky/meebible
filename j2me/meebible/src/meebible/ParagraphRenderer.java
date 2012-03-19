package meebible;

import javax.microedition.lcdui.Font;
import javax.microedition.lcdui.Graphics;

public class ParagraphRenderer {
    
    public static final int SIDE_MARGIN = 3;
    
    class Drawer implements MarkupListener {
        private String markup;
        private RenderCanvas canvas;
        private Graphics graphics;
        private int yStart = 0;
        private int drawTop = 0;
        private int drawHeight = 20;

        int caretX;
        int caretY;
        int prevColor;
        
        Drawer(String markup, RenderCanvas canvas) {
            this.markup = markup;
            this.canvas = canvas;
        }
        
        int calcHeight() {
            this.graphics = null;
            new MarkupParser(markup).parse(this);
            
            return caretY + canvas.getFont().getHeight() + canvas.getFont().getHeight() / 2;
        }
        
        void draw(Graphics graphics, int yStart, int drawTop, int drawHeight) {
            this.graphics = graphics;
            this.yStart = yStart;
            this.drawTop = drawTop;
            this.drawHeight = drawHeight;
            new MarkupParser(markup).parse(this);
        }
        
        public void reset() {
            caretX = SIDE_MARGIN + 3 * canvas.getFont().charWidth(' ');
            caretY = yStart;
            prevColor = -1;
        }
        
        private void handleWord(String word, int color) {
            int wordWidth = canvas.getFont().stringWidth(word);
            
            if (caretX + wordWidth >= canvas.getWidth() - SIDE_MARGIN) {
                caretX = SIDE_MARGIN;
                caretY += canvas.getFont().getHeight();
            }
            
            if (graphics != null) {
                if (color != prevColor)
                {
                    graphics.setColor(color);
                    prevColor = color;
                }
                
                if (caretY + canvas.getFont().getHeight() > drawTop  &&  caretY < drawTop + drawHeight)
                    graphics.drawString(word, caretX, caretY, 0);
            }
            
            caretX += wordWidth + canvas.getFont().charWidth(' ');
        }
        
        public void word(String word) {
            handleWord(word, 0x202020);
        }
        
        public void verseNumber(String number) {
            handleWord(number, 0x808080);
        }
    }
    
    
    RenderCanvas canvas;
    
    
    ParagraphRenderer(RenderCanvas canvas) {
        this.canvas = canvas;
    }
    
    
    int parHeight(String markup) {
        return new Drawer(markup, canvas).calcHeight();
    }
    
    void drawPar(String markup, Graphics graphics, int yStart, int drawTop, int drawHeight) {
        new Drawer(markup, canvas).draw(graphics, yStart, drawTop, drawHeight);
    }
}
