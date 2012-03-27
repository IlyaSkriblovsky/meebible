#include "ICUUnicodeSearch.h"

#include <unicode/ucol.h>
#include <unicode/ucnv.h>
#include <unicode/usearch.h>

#include <cassert>


UCollator* ICUUnicodeSearch::_icuCollator = 0;

ICUUnicodeSearch::ICUUnicodeSearch(const char16* haystack, const char16* needle)
    : UnicodeSearch(haystack, needle), _search(0)
{
    if (_icuCollator == 0)
    {
        UErrorCode err = U_ZERO_ERROR;
        _icuCollator = ucol_open("", &err);
        ucol_setStrength(_icuCollator, UCOL_PRIMARY);
    }
}

ICUUnicodeSearch::~ICUUnicodeSearch()
{
    if (_search)
        usearch_close(_search);
}

int ICUUnicodeSearch::start()
{
    if (_search)
        usearch_close(_search);

    UErrorCode err = U_ZERO_ERROR;
    _search = usearch_openFromCollator(
        static_cast<const UChar*>(haystack()), -1,
        static_cast<const UChar*>(needle()), -1,
        _icuCollator, 0, &err
    );

    int pos = usearch_first(_search, &err);
    return pos == USEARCH_DONE ? -1 : pos;
}

int ICUUnicodeSearch::next()
{
    assert(_search != 0);

    UErrorCode err = U_ZERO_ERROR;

    int pos = usearch_next(_search, &err);
    return pos == USEARCH_DONE ? -1 : pos;
}

int ICUUnicodeSearch::matchLength()
{
    assert(_search != 0);

    return usearch_getMatchedLength(_search);
}
