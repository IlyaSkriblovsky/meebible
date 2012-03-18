package rendertest;

class Book {
    public final String code;
    public final String name;
    public final int[] verseCount;

    public Book(String code, String name, int[] verseCount) {
        this.code = code;
        this.name = name;
        this.verseCount = verseCount;
    }

    public String toString() {
        return "(" + code + ") " + name;
    }
}
