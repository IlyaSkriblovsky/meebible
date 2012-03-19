package meebible;

public class MarkupParser {
    
    public final String markup;
    
    MarkupParser(String markup) {
        this.markup = markup;
    }
    
        
    
    private boolean isWordChar(char c) {
        return c != ' ' && c != '{' && c != '|' && c != '\n' && c != '\r';
    }

    public void parse(MarkupListener listener) {
        listener.reset();
        
        int pos = 0;
        while (pos < markup.length()) {
            char c = markup.charAt(pos);
            
            if (c == '{') {
                int closeBrace = pos;
                while (markup.charAt(++closeBrace) != '}') { }
                
                listener.verseNumber(markup.substring(pos+1, closeBrace));
                
                pos = closeBrace + 1;
            } else {
                if (! isWordChar(markup.charAt(pos)))
                    pos++;
                else {
                    int start = pos;

                    while (pos < markup.length()) {
                        if (markup.charAt(pos) == '\\') pos++;
                        if (! isWordChar(markup.charAt(pos))) break;
                        pos++;
                    }

                    listener.word(markup.substring(start, pos));
                }
            }
        }
    }
}
