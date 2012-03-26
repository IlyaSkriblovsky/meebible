#include "SqliteUnicodeSearch.h"

#include <QSqlDriver>
#include <QVariant>
#include <QDebug>

#include <sqlite3.h>

#include "UnicodeCollator.h"

#ifdef SEARCH_ICU
    #include "ICUUnicodeCollator.h"
#endif
#ifdef SEARCH_SYMBIAN
    #include "SymbianUnicodeCollator.h"
#endif
#ifdef SEARCH_SIMPLE
    #include "SimpleUnicodeCollator.h"
#endif


Q_DECLARE_METATYPE(sqlite3*)


UnicodeCollator *collator = 0;


void unicode_like(sqlite3_context *ctx, int n, sqlite3_value **args);
void matchCount(sqlite3_context *ctx, int n, sqlite3_value **args);


void SqliteUnicodeSearch::installUnicodeSearch(const QSqlDatabase& db)
{
    sqlite3* sqdb = db.driver()->handle().value<sqlite3*>();

    if (collator != 0)
    {
        delete collator;
        collator = 0;
    }


    #ifdef SEARCH_ICU
        #define __SEARCH_COLLATOR_CREATED
        qDebug() << "Using ICU search";
        collator = new ICUUnicodeCollator;
    #endif

    #ifdef SEARCH_SYMBIAN
        #define __SEARCH_COLLATOR_CREATED
        qDebug() << "Using Symbian search";
        collator = new SymbianUnicodeCollator;
    #endif

    #ifdef SEARCH_SIMPLE
        #define __SEARCH_COLLATOR_CREATED
        qDebug() << "Using simple search";
        collator = new SimpleUnicodeCollator;
    #endif

    #ifndef __SEARCH_COLLATOR_CREATED
        #error "Define search collatir by search-* qmake config options";
    #endif


    sqlite3_create_function(
        sqdb, "unicodeMatch", 2,
        SQLITE_ANY, collator,
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

    UnicodeCollator *collator = reinterpret_cast<UnicodeCollator*>(sqlite3_user_data(ctx));
    const char16* needle = reinterpret_cast<const char16*>(sqlite3_value_text16(args[0]));
    const char16* haystack = reinterpret_cast<const char16*>(sqlite3_value_text16(args[1]));

    int firstMatch;
    count = collator->find(haystack, needle, &firstMatch);

    if (count == 0)
        sqlite3_result_null(ctx);
    else
    {
        int begin = firstMatch > 20 ? firstMatch - 20 : 0;
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


QString SqliteUnicodeSearch::highlightMatches(const QString& html, const QString& needle, int* matchCount)
{
    if (needle == "") {
        *matchCount = 0;
        return html;
    }


    return collator->markMatches(html, needle, matchCount);
}
