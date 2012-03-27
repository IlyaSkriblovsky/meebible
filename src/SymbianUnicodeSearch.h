#ifndef SYMBIANUNICODESEARCH_h
#define SYMBIANUNICODESEARCH_h

#include "UnicodeSearch.h"

#include <platform/collate.h>

class SymbianUnicodeSearch: public UnicodeSearch
{
    public:
        SymbianUnicodeSearch(const char16* haystack, const char16* needle);
        virtual ~SymbianUnicodeSearch();

        virtual int start();
        virtual int next();
        virtual int matchLength();


    private:
        TPtrC16 _p_needle;

        int _curOffset;
        TPtrC16 _p_cur;

        TCollationMethod _collationMethod;
        int _matchLength;
};

#endif
