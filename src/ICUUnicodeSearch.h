#ifndef ICUUNICODESEARCH_H
#define ICUUNICODESEARCH_H

#include "UnicodeSearch.h"

class UCollator;
class UStringSearch;

class ICUUnicodeSearch: public UnicodeSearch
{
    public:
        ICUUnicodeSearch(const char16* haystack, const char16* needle);
        virtual ~ICUUnicodeSearch();

        virtual int start();
        virtual int next();
        virtual int matchLength();

    private:
        static UCollator* _icuCollator;
        UStringSearch* _search;
};

#endif
