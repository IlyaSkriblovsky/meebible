package meebible;

import javax.microedition.lcdui.Font;
import javax.microedition.lcdui.Graphics;

public class ParagraphRenderer {
    
    public static final int SIDE_MARGIN = 3;
    
    class Drawer implements MarkupParser.MarkupListener {
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
        
        int searchingForVerse;
        
        Drawer(String markup, RenderCanvas canvas) {
            this.markup = markup;
            this.canvas = canvas;
        }
        
        int calcHeight() {
            this.graphics = null;
            this.searchingForVerse = -1;
            new MarkupParser(markup).parse(this);
            
            int height = caretY;
            if (font != null)
                height += font.getHeight() + font.getHeight() / 2;
            return height;
        }
        
        int verseOffset(int verseNo) {
            this.graphics = null;
            this.searchingForVerse = verseNo;
            new MarkupParser(markup).parse(this);
            return caretY;
        }
        
        void draw(Graphics graphics, int yStart, int drawTop, int drawHeight) {
            this.graphics = graphics;
            this.searchingForVerse = -1;
            this.yStart = yStart;
            this.drawTop = drawTop;
            this.drawHeight = drawHeight;
            
            graphics.setFont(canvas.getFont(Font.STYLE_PLAIN));
            
            new MarkupParser(markup).parse(this);
        }
        
        public void reset() {
            caretX = SIDE_MARGIN + 3 * canvas.getFont().charWidth(' ');
            caretY = yStart;
            prevColor = -1;
            
            font = canvas.getFont(Font.STYLE_PLAIN);
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
        
        public boolean verseNumber(String number, int fontStyle) {
            if (searchingForVerse != -1 && searchingForVerse == Integer.parseInt(number))
                return false;
            
            handleWord(number, 0x808080, fontStyle);
            return true;
        }
        
        public void lineBreak(int indentWidth) {
            caretX = SIDE_MARGIN + indentWidth * font.charWidth(' ');
            caretY += font.getHeight();
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
    
    int verseOffset(String markup, int verseNo) {
        return new Drawer(markup, canvas).verseOffset(verseNo);
    }
}
