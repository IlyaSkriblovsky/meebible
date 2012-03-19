package meebible;

class Book {
    public final String code;
    public final String name;
    public final int chapterCount;

    public Book(String code, String name, int chapterCount) {
        this.code = code;
        this.name = name;
        this.chapterCount = chapterCount;
    }

    public String toString() {
        return "(" + code + ", " + chapterCount + ") " + name;
    }
}
