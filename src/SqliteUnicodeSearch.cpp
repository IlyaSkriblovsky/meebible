#include "SqliteUnicodeSearch.h"

#include <QSqlDriver>
#include <QVariant>
#include <QDebug>

#include <sqlite3.h>

#include <unicode/ucol.h>
#include <unicode/ucnv.h>
#include <unicode/usearch.h>


Q_DECLARE_METATYPE(sqlite3*)


UCollator *collator = 0;


void unicode_like(sqlite3_context *ctx, int n, sqlite3_value **args);
void matchCount(sqlite3_context *ctx, int n, sqlite3_value **args);


void SqliteUnicodeSearch::installUnicodeSearch(const QSqlDatabase& db)
{
    sqlite3* sqdb = db.driver()->handle().value<sqlite3*>();

    if (collator != 0)
        ucol_close(collator);

    UErrorCode err = U_ZERO_ERROR;
    collator = ucol_open("", &err);
    ucol_setStrength(collator, UCOL_PRIMARY);

    sqlite3_create_function(
        sqdb,
        "unicodeMatch",
        2,
        SQLITE_ANY,
        collator,
        unicode_like,
        0,
        0
    );

    sqlite3_create_function(
        sqdb,
        "matchCount",
        0,
        SQLITE_ANY,
        0,
        matchCount,
        0,
        0
    );
}


int count;

void unicode_like(sqlite3_context *ctx, int n, sqlite3_value **args)
{
    Q_UNUSED(n);
    Q_ASSERT(n == 2);

    UErrorCode err = U_ZERO_ERROR;

    UCollator *collator = (UCollator*)sqlite3_user_data(ctx);

    UChar* needle = (UChar*)sqlite3_value_text16(args[0]);
    UChar* haystack = (UChar*)sqlite3_value_text16(args[1]);

    UStringSearch *search = usearch_openFromCollator(
        needle,
        -1,
        haystack,
        -1,
        collator,
        0,
        &err
    );



    count = 0;

    int pos = usearch_first(search, &err);
    int firstpos = pos;

    while (pos != USEARCH_DONE)
    {
        count++;
        pos = usearch_next(search, &err);
    }

    usearch_close(search);

    if (firstpos == USEARCH_DONE)
        sqlite3_result_null(ctx);
    else
    {
        int begin = firstpos > 20 ? firstpos - 20 : 0;
        int textlen = sqlite3_value_bytes16(args[1]) / 2;
        int len = begin + 45 <= textlen ? 45 : textlen - begin;
        sqlite3_result_text16(ctx, &haystack[begin], len * 2, 0);
    }
}



void matchCount(sqlite3_context *ctx, int n, sqlite3_value **args)
{
    Q_UNUSED(n)
    Q_UNUSED(args)
    sqlite3_result_int(ctx, count);
}


QString SqliteUnicodeSearch::highlightMatches(const QString& html, const QString& needle)
{
    if (needle == "")
        return html;

    UErrorCode err = U_ZERO_ERROR;

    UChar* needle_c = (UChar*)needle.utf16();
    UChar* html_c = (UChar*)html.utf16();

    UStringSearch *search = usearch_openFromCollator(
        needle_c, -1,
        html_c, -1,
        collator,
        0, &err
    );


    QString result = html;

    int pos = usearch_last(search, &err);

    while (pos != USEARCH_DONE)
    {
        result.insert(pos + usearch_getMatchedLength(search), "</span>");
        result.insert(pos, "<span class=\"match\">");

        pos = usearch_previous(search, &err);
    }


    usearch_close(search);

    return result;
}
