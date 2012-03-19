package meebible;

class Book {
    public final String code;
    public final String name;

    public Book(String code, String name) {
        this.code = code;
        this.name = name;
    }

    public String toString() {
        return "(" + code + ") " + name;
    }
}
