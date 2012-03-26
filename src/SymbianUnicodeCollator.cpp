#include "SymbianUnicodeCollator.h"

#include <QDebug>

#include <platform/collate.h>


int SymbianUnicodeCollator::find(const char16* haystack, const char16* needle, int* firstMatch)
{
    TPtrC16 ph(haystack);
    TPtrC16 pn(needle);

    int lengthFound;
    TCollationMethod collationMethod = *Mem::CollationMethodByIndex(0);
    collationMethod.iFlags |= TCollationMethod::EIgnoreNone;

    int count = 0;
    int pos = ph.FindC(pn, lengthFound, collationMethod, 0);
    *firstMatch = pos != KErrNotFound ? pos : -1;

    while (pos != KErrNotFound)
    {
        count++;
        ph.Set(ph.Mid(pos+1));
        pos = ph.FindC(pn, lengthFound, collationMethod, 0);
    }

    return count;
}

QString SymbianUnicodeCollator::markMatches(const QString& html, const QString& needle, int* matchCount)
{
    QString result = html;

    int lengthFound;
    TCollationMethod collationMethod = *Mem::CollationMethodByIndex(0);
    collationMethod.iFlags |= TCollationMethod::EIgnoreNone;

    TPtrC16 pn(needle.utf16());

    int count = 0;
    {
        TPtrC16 ph(html.utf16());
        TPtrC16 pn(needle.utf16());
        int pos = ph.FindC(pn, lengthFound, collationMethod, 0);
        while (pos != KErrNotFound)
        {
            count++;
            ph.Set(ph.Mid(pos + 1));
            pos = ph.FindC(pn, lengthFound, collationMethod, 0);
        }
    }

    TPtrC16 ph(html.utf16());

    int pos = ph.FindC(pn, lengthFound, collationMethod, 0);
    int totalPos = 0;
    int resultPos = 0;
    int i = count;
    while (pos != KErrNotFound)
    {
        totalPos += pos;
        resultPos += pos;
        if (html.indexOf("<", totalPos) < html.indexOf(">", totalPos))
        {
            QString before = QString("<span id=\"match-%1\" class=\"match\">").arg(i);
            QString after = "</span>";
            result.insert(resultPos + lengthFound, after);
            result.insert(resultPos, before);

            // Counting backwards
            i--;

            resultPos += before.length();
            resultPos += after.length();
        }

        totalPos += 1;
        resultPos += 1;

        ph.Set(ph.Mid(pos + 1));
        pos = ph.FindC(pn, lengthFound, collationMethod, 0);
    }

    if (matchCount)
        *matchCount = count;
    return result;
}
