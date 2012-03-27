#include "SymbianUnicodeSearch.h"



SymbianUnicodeSearch::SymbianUnicodeSearch(const char16* haystack, const char16* needle)
    : UnicodeSearch(haystack, needle), _p_needle(needle), _matchLength(0)
{
    _collationMethod = *Mem::CollationMethodByIndex(0);
    _collationMethod.iFlags |= TCollationMethod::EIgnoreNone;
}

SymbianUnicodeSearch::~SymbianUnicodeSearch()
{
}


int SymbianUnicodeSearch::start()
{
    _curOffset = 0;
    _p_cur.Set(TPtrC16(haystack()));

    int pos = _p_cur.FindC(_p_needle, _matchLength, _collationMethod, 0);

    if (pos != KErrNotFound)
    {
        _curOffset += pos+1;
        _p_cur.Set(_p_cur.Mid(pos+1));
    }

    return pos == KErrNotFound ? -1 : pos;
}

int SymbianUnicodeSearch::next()
{
    int pos = _p_cur.FindC(_p_needle, _matchLength, _collationMethod, 0);

    int result = _curOffset + pos;

    if (pos != KErrNotFound)
    {
        _curOffset += pos+1;
        _p_cur.Set(_p_cur.Mid(pos+1));
    }

    return pos == KErrNotFound ? -1 : result;
}

int SymbianUnicodeSearch::matchLength()
{
    return _matchLength;
}
