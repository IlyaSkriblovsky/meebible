#include "SimpleUnicodeCollator.h"


int SimpleUnicodeCollator::find(const char16* haystack, const char16* needle, int* firstMatch)
{
    int count = 0;

    QString hs = QString::fromUtf16(haystack);
    QString ns = QString::fromUtf16(needle);

    int pos = hs.indexOf(ns, 0, Qt::CaseInsensitive);
    *firstMatch = pos;

    while (pos != -1)
    {
        count++;
        pos = hs.indexOf(ns, pos+1, Qt::CaseInsensitive);
    }

    return count;
}


QString SimpleUnicodeCollator::markMatches(const QString& html, const QString& needle, int* matchCount)
{
    QString result = html;

    int count = 0;
    int pos = html.lastIndexOf(needle, -1, Qt::CaseInsensitive);

    while (pos != -1)
    {
        if (html.indexOf("<", pos) < html.indexOf(">", pos))
        {
            result.insert(pos + needle.length(), "</span>");
            result.insert(pos, QString("<span id=\"match-%1\" class=\"match\">").arg(count + 1));

            count++;
        }

        pos = html.lastIndexOf(needle, pos-1, Qt::CaseInsensitive);
    }

    *matchCount = count;
    return result;
}
