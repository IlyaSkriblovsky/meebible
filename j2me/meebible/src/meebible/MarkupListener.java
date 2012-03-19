package meebible;

public interface MarkupListener {
    void reset();
    void word(String word);
    void verseNumber(String number);
}
