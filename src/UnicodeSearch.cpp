#include "UnicodeSearch.h"

#ifdef SEARCH_ICU
    #include "ICUUnicodeSearch.h"
    #define UNICODE_SEARCH_CLASS ICUUnicodeSearch
#endif

#ifdef SEARCH_SYMBIAN
    #include "SymbianUnicodeSearch.h"
    #define UNICODE_SEARCH_CLASS SymbianUnicodeSearch
#endif

#ifndef UNICODE_SEARCH_CLASS
    #error "Search method not defined"
#endif


UnicodeSearch* UnicodeSearch::create(const char16* haystack, const char16* needle)
{
    return new UNICODE_SEARCH_CLASS(haystack, needle);
}


UnicodeSearch::UnicodeSearch(const char16* haystack, const char16* needle)
    :_haystack(haystack), _needle(needle)
{
}

UnicodeSearch::~UnicodeSearch()
{
}
