#ifndef UNICODESEARCH_H
#define UNICODESEARCH_H

typedef unsigned short char16;

class UnicodeSearch
{
    public:
        static UnicodeSearch* create(const char16* haystack, const char16* needle);

        UnicodeSearch(const char16* haystack, const char16* needle);
        virtual ~UnicodeSearch();

        const char16* haystack() const { return _haystack; }
        const char16* needle() const { return _needle; }

        virtual int start() = 0;
        virtual int next() = 0;
        virtual int matchLength() = 0;

    private:
        const char16* _haystack;
        const char16* _needle;
};

#endif
