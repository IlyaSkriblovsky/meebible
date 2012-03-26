#include "ICUUnicodeCollator.h"

#include <QDebug>

#include <unicode/ucol.h>
#include <unicode/ucnv.h>
#include <unicode/usearch.h>


ICUUnicodeCollator::ICUUnicodeCollator()
    :_icuCollator(0)
{
    UErrorCode err = U_ZERO_ERROR;
    _icuCollator = ucol_open("", &err);
    ucol_setStrength(_icuCollator, UCOL_PRIMARY);
}

ICUUnicodeCollator::~ICUUnicodeCollator()
{
    ucol_close(_icuCollator);
}


int ICUUnicodeCollator::find(const char16* haystack, const char16* needle, int* firstMatch)
{
    UErrorCode err = U_ZERO_ERROR;

    UStringSearch *search = usearch_openFromCollator(
        (UChar*)needle, -1,
        (UChar*)haystack, -1,
        _icuCollator, 0, &err
    );


    int pos = usearch_first(search, &err);
    int firstpos = pos;
    int count = 0;

    while (pos != USEARCH_DONE)
    {
        count++;
        pos = usearch_next(search, &err);
    }

    usearch_close(search);

    if (firstpos == USEARCH_DONE)
    {
        *firstMatch = -1;
        return 0;
    }
    else
    {
        *firstMatch = firstpos;
        return count;
    }
}


QString ICUUnicodeCollator::markMatches(const QString& html, const QString& needle, int* matchCount)
{
    UErrorCode err = U_ZERO_ERROR;

    UChar* needle_c = (UChar*)needle.utf16();
    UChar* html_c = (UChar*)html.utf16();

    UStringSearch *search = usearch_openFromCollator(
        needle_c, -1,
        html_c, -1,
        _icuCollator,
        0, &err
    );


    QString result = html;

    int count = 0;
    int pos = usearch_last(search, &err);

    while (pos != USEARCH_DONE)
    {
        if (html.indexOf("<", pos) < html.indexOf(">", pos)) // this implies != -1
        {
            result.insert(pos + usearch_getMatchedLength(search), "</span>");
            result.insert(pos, QString("<span id=\"match-%1\" class=\"match\">").arg(count + 1));

            count++;
        }

        pos = usearch_previous(search, &err);
    }


    usearch_close(search);

    if (matchCount != 0)
        *matchCount = count;

    return result;
}
