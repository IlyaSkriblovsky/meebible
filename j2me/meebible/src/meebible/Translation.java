package meebible;

class Translation {
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
