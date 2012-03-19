package meebible;

import javax.microedition.lcdui.Font;

public class MarkupParser {
    
    public interface MarkupListener {
        void reset();
        void word(String word, int fontStyle);
        boolean verseNumber(String number, int fontStyle);
        void lineBreak(int indentWidth);
    }

    
    public final String markup;
    
    MarkupParser(String markup) {
        this.markup = markup;
    }
    
        
    
    private boolean isWordChar(char c) {
        return c != ' ' && c != '{' && c != '|' && c != '\n' && c != '\r' && c != '^';
    }

    public void parse(MarkupListener listener) {
        int fontStyle = Font.STYLE_PLAIN;
        
        listener.reset();
        
        int pos = 0;
        while (pos < markup.length()) {
            char c = markup.charAt(pos);
            
            if (c == '{') {
                int closeBrace = pos;
                while (markup.charAt(++closeBrace) != '}') { }
                
                if (! listener.verseNumber(markup.substring(pos+1, closeBrace), fontStyle))
                    return;
                
                pos = closeBrace + 1;
            }
            if (c == '^') {
                pos++;
                char styleChar = markup.charAt(pos++);
                switch (styleChar) {
                    case 'B': fontStyle |= Font.STYLE_BOLD; break;
                    case 'b': fontStyle &= ~Font.STYLE_BOLD; break;
                    case 'I': fontStyle |= Font.STYLE_ITALIC; break;
                    case 'i': fontStyle &= ~Font.STYLE_ITALIC; break;
                    case 'U': fontStyle |= Font.STYLE_UNDERLINED; break;
                    case 'u': fontStyle &= ~Font.STYLE_UNDERLINED; break;
                        
                    case 'N': listener.lineBreak(3); break;
                    case 'n': listener.lineBreak(0); break;
                }
            }
            else {
                if (! isWordChar(markup.charAt(pos)))
                    pos++;
                else {
                    StringBuffer buf = new StringBuffer();

                    while (pos < markup.length()) {
                        if (markup.charAt(pos) == '\\') {
                            pos++;
                            buf.append(markup.charAt(pos));
                        }
                        else {
                            if (! isWordChar(markup.charAt(pos))) break;
                            buf.append(markup.charAt(pos));
                        }
                        pos++;
                    }

                    listener.word(buf.toString(), fontStyle);
                }
            }
        }
    }
}
