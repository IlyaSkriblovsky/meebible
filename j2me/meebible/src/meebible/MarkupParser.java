package meebible;

import javax.microedition.lcdui.Font;

public class MarkupParser {
    
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
                
                listener.verseNumber(markup.substring(pos+1, closeBrace), fontStyle);
                
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
                }
            }
            else {
                if (! isWordChar(markup.charAt(pos)))
                    pos++;
                else {
                    int start = pos;

                    while (pos < markup.length()) {
                        if (markup.charAt(pos) == '\\') pos++;
                        else
                            if (! isWordChar(markup.charAt(pos))) break;
                        pos++;
                    }

                    listener.word(markup.substring(start, pos), fontStyle);
                }
            }
        }
    }
}
