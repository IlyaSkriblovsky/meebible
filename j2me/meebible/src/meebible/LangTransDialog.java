package meebible;

import java.util.Enumeration;
import java.util.Vector;
import javax.microedition.lcdui.*;

public class LangTransDialog {

    static abstract class Listener {
        public abstract void selected(String langCode, String transCode, Book[] books);
        public void cancelled() { }
    }
    
    
    class LanguageList extends List implements CommandListener {
        Command cmdSelect = new Command("Select", Command.ITEM, 1);
        Command cmdBack = new Command("Back", Command.BACK, 1);

        LanguageList() {
            super("Select language", Choice.IMPLICIT);
            
            for (Enumeration e = languages.elements(); e.hasMoreElements();) {
                Language lang = (Language)e.nextElement();
                append(lang.toString(), null);
            }

            setCommandListener(this);
            addCommand(cmdBack);
            setSelectCommand(cmdSelect);
        }

        public void commandAction(Command c, Displayable d) {
            if (c == cmdSelect) {
                Language lang = (Language)languages.elementAt(getSelectedIndex());
                MeeBibleMidlet.instance.show(new TranslationList(this, lang));
            }
            else if (c == cmdBack)
                listener.cancelled();
        }
    }
    
    class TranslationList extends List implements CommandListener {
        LanguageList langList;
        Language lang;
        Command cmdSelect = new Command("Select", Command.ITEM, 1);
        Command cmdBack = new Command("Back", Command.BACK, 1);
        
        TranslationList(LanguageList langList, Language lang) {
            super("Select translation", Choice.IMPLICIT);
            this.langList = langList;
            this.lang = lang;
            
            for (Enumeration e = lang.translations.elements(); e.hasMoreElements();) {
                Translation translation = (Translation)e.nextElement();
                append(translation.name, null);
            }
            
            addCommand(cmdBack);
            
            setCommandListener(this);
            setSelectCommand(cmdSelect);
        }
        
        public void commandAction(Command c, Displayable d) {
            if (c == cmdBack)
                MeeBibleMidlet.instance.show(langList);
            else if (c == cmdSelect) {
                final Translation trans = (Translation)lang.translations.elementAt(getSelectedIndex());
                
                final LoadingSplash splash = new LoadingSplash("Loading books...");
                
                Loader.load("http://meebible.org/translation.j2me?lang=" + lang.code + "&trans=" + trans.code, new LoadListener() {
                    public void finished(String content) {
                        final Vector v_books = new Vector();
                        
                        Utils.forEachStringPart(content, '\n', new Utils.StringPartCallback() {
                            public void onPart(String part) {
                                if (part.length() == 0) return;
                                
                                String[] parts = Utils.split(part, '|');
//                                String[] verseCountsStr = Utils.split(parts[2], ',');
//                                int[] verseCounts = new int[verseCountsStr.length];
//                                for (int i = 0; i < verseCounts.length; i++)
//                                    verseCounts[i] = Integer.parseInt(verseCountsStr[i]);
                                
                                v_books.addElement(new Book(parts[0], parts[1])); //, verseCounts));
                            }
                        });
                        
                        Book[] books = new Book[v_books.size()];
                        int i = 0;
                        for (Enumeration e = v_books.elements(); e.hasMoreElements();)
                            books[i++] = (Book)e.nextElement();
                        
                        listener.selected(lang.code, trans.code, books);
                    }
                    
                    public void error() {
                        // FIXME
                        throw new UnsupportedOperationException("Not supported yet.");
                    }
                });
            }
        }
    }

    
    Vector languages = new Vector();
    
    Listener listener;
    
    public void open(Listener listener) {
        this.listener = listener;
        reload();
    }
    
    
    

    public void reload() {
        final LoadingSplash splash = new LoadingSplash("Loading languages...");

        Loader.load("http://meebible.org/meta.j2me", new LoadListener() {
            public void finished(String content) {
                languages.removeAllElements();
                
                Utils.forEachStringPart(content, '\n', new Utils.StringPartCallback() {
                    String transCode;
                    String transSourceUrl;
                    String transCopyright;
                    boolean transRtl;
                    
                    public void onPart(String line) {
                        String[] parts = Utils.split(line, '|');
                            
                        if (line.startsWith("lang|")) {
                            languages.addElement(new Language(parts[1], parts[2], parts[3]));
                        }
                        else if (line.startsWith("trans|")) {
                            transCode = parts[1];
                            transSourceUrl = parts[2];
                            transCopyright = parts[3];
                            transRtl = parts[4].equals("1");
                        }
                        else if (line.startsWith("transLang|")) {
                            int i = 0;
                            for (Enumeration e = languages.elements(); e.hasMoreElements();) {
                                if (((Language)e.nextElement()).code.equals(parts[1])) break;
                                i++;
                            }
                            Language lang = (Language)languages.elementAt(i);
                            lang.addTranslation(new Translation(
                                lang, transCode, parts[2], transSourceUrl, transCopyright, transRtl
                            ));
                        }
                    }
                });

                splash.close(new LanguageList());
            }

            public void error() {
                // FIXME
                throw new UnsupportedOperationException("Not supported yet.");
            }
        });
    }
}
