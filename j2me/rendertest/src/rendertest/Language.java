package rendertest;

import java.util.Vector;

class Language {
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
