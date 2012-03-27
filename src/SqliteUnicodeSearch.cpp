#include "SqliteUnicodeSearch.h"

#include <QSqlDriver>
#include <QVariant>
#include <QDebug>

#include <sqlite3.h>

#include "UnicodeSearch.h"


Q_DECLARE_METATYPE(sqlite3*)


void unicode_like(sqlite3_context *ctx, int n, sqlite3_value **args);
void matchCount(sqlite3_context *ctx, int n, sqlite3_value **args);


void SqliteUnicodeSearch::installUnicodeSearch(const QSqlDatabase& db)
{
    sqlite3* sqdb = db.driver()->handle().value<sqlite3*>();

    sqlite3_create_function(
        sqdb, "unicodeMatch", 2,
        SQLITE_ANY, 0,
        unicode_like, 0, 0
    );

    sqlite3_create_function(
        sqdb, "matchCount", 0,
        SQLITE_ANY, 0,
        matchCount, 0, 0
    );
}


int count;

void unicode_like(sqlite3_context *ctx, int n, sqlite3_value **args)
{
    Q_UNUSED(n);
    Q_ASSERT(n == 2);

    const char16* needle = reinterpret_cast<const char16*>(sqlite3_value_text16(args[0]));
    const char16* haystack = reinterpret_cast<const char16*>(sqlite3_value_text16(args[1]));

    UnicodeSearch* usearch = UnicodeSearch::create(haystack, needle);

    count = 0;
    int pos = usearch->start();
    int firstMatch = pos;

    while (pos != -1)
    {
        count++;
        pos = usearch->next();
    }


    if (count == 0)
        sqlite3_result_null(ctx);
    else
    {
        int begin = firstMatch > 20 ? firstMatch - 20 : 0;
        int textlen = sqlite3_value_bytes16(args[1]) / 2;
        int len = begin + 45 <= textlen ? 45 : textlen - begin;
        sqlite3_result_text16(ctx, &haystack[begin], len * 2, 0);
    }

    delete usearch;
}



void matchCount(sqlite3_context *ctx, int n, sqlite3_value **args)
{
    Q_UNUSED(n)
    Q_UNUSED(args)
    sqlite3_result_int(ctx, count);
}


QString SqliteUnicodeSearch::highlightMatches(const QString& html, const QString& needle, int* matchCount)
{
    if (needle == "") {
        *matchCount = 0;
        return html;
    }


    const char16* haystack_c = html.utf16();
    const char16* needle_c = needle.utf16();

    UnicodeSearch* usearch = UnicodeSearch::create(haystack_c, needle_c);

    int count = 0;
    {
        int pos = usearch->start();
        while (pos != -1)
        {
            count++;
            pos = usearch->next();
        }
    }


    QString result = html;

    int pos = usearch->start();
    int prevPos = 0;
    int resultPos = 0;
    int i = count;
    while (pos != -1)
    {
        resultPos += pos - prevPos;
        if (html.indexOf("<", pos) < html.indexOf(">", pos))
        {
            QString before = QString("<span id=\"match-%1\" class=\"match\">").arg(i);
            QString after = "</span>";
            result.insert(resultPos + usearch->matchLength(), after);
            result.insert(resultPos, before);

            i--;

            resultPos += before.length();
            resultPos += after.length();
        }

        resultPos += 1;

        prevPos = pos;
        pos = usearch->next();
    }

    if (matchCount) *matchCount = count;

    delete usearch;

    return result;
}
