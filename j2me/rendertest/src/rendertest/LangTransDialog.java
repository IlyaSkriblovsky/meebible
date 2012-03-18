package rendertest;

import java.util.Enumeration;
import java.util.Vector;
import javax.microedition.lcdui.*;

public class LangTransDialog {

    static class Language {
        public final String code;
        public final String engname;
        public final String selfname;

        public Vector translations;

        public Language(String code, String engname, String selfname) {
            this.code = code;
            this.engname = engname;
            this.selfname = selfname;
            
            translations = new Vector();
        }

        public void addTranslation(Translation translation) {
            translations.addElement(translation);
        }

        public Translation getTranslation(int i) {
            return (Translation)translations.elementAt(i);
        }

        public String toString() { 
            return engname + " (" + selfname + ")";
        }
    }
    
    static class Translation {
        public final Language language;
        public final String code;
        public final String name;
        public final String sourceUrl;
        public final String copyright;
        public final boolean rtl;
        
        public Translation(Language language, String code, String name, String sourceUrl, String copyright, boolean rtl) {
            this.language = language;
            this.code = code;
            this.name = name;
            this.sourceUrl = sourceUrl;
            this.copyright = copyright;
            this.rtl = rtl;
        }
    }
    
    
    static abstract class Listener {
        public abstract void selected(String langCode, String transCode);
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
                RenderMidlet.instance.show(new TranslationList(this, lang));
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
                RenderMidlet.instance.show(langList);
            else if (c == cmdSelect)
                listener.selected(lang.code, ((Translation)lang.translations.elementAt(getSelectedIndex())).code);
        }
    }

    
    Vector languages = new Vector();
    
    Listener listener;
    
    public void open(Listener listener) {
        this.listener = listener;
        reload();
    }
    
    
    

    public void reload() {
        final LoadingSplash splash = new LoadingSplash("Loading...");

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
                throw new UnsupportedOperationException("Not supported yet.");
            }
        });
    }
}
