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
        Font font;
        
        Drawer(String markup, RenderCanvas canvas) {
            this.markup = markup;
            this.canvas = canvas;
        }
        
        int calcHeight() {
            this.graphics = null;
            new MarkupParser(markup).parse(this);
            
            int height = caretY;
            if (font != null)
                height += font.getHeight() + font.getHeight() / 2;
            return height;
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
            
            font = null;
        }
        
        private void handleWord(String word, int color, int fontStyle) {
            if (font == null || fontStyle != font.getStyle()) {
                font = canvas.getFont(fontStyle);
                if (graphics != null)
                    graphics.setFont(font);
            }
            
            int wordWidth = font.stringWidth(word);
            
            if (caretX + wordWidth >= canvas.getWidth() - SIDE_MARGIN) {
                caretX = SIDE_MARGIN;
                caretY += font.getHeight();
            }
            
            if (graphics != null) {
                if (color != prevColor)
                {
                    graphics.setColor(color);
                    prevColor = color;
                }
                
                if (caretY + font.getHeight() > drawTop  &&  caretY < drawTop + drawHeight)
                    graphics.drawString(word, caretX, caretY, 0);
            }
            
            caretX += wordWidth;
            
            if (graphics != null && (font.getStyle() & Font.STYLE_UNDERLINED) != 0)
                graphics.drawChar(' ', caretX, caretY, 0);
            
            caretX += font.charWidth(' ');
        }
        
        public void word(String word, int fontStyle) {
            handleWord(word, 0x202020, fontStyle);
        }
        
        public void verseNumber(String number, int fontStyle) {
            handleWord(number, 0x808080, fontStyle);
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
