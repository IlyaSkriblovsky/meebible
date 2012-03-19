package meebible;

public interface MarkupListener {
    void reset();
    void word(String word, int fontStyle);
    void verseNumber(String number, int fontStyle);
}
