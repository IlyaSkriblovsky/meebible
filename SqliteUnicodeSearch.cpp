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


void SqliteUnicodeSearch::installUnicodeSearch(const QSqlDatabase& db)
{
    sqlite3* sqdb = db.driver()->handle().value<sqlite3*>();

    qDebug() << "Threading:" << sqlite3_threadsafe();

    if (collator != 0)
        ucol_close(collator);

    UErrorCode err = U_ZERO_ERROR;
    collator = ucol_open("", &err);
    ucol_setStrength(collator, UCOL_PRIMARY);

    sqlite3_create_function(
        sqdb,
        "like",
        2,
        SQLITE_ANY,
        collator,
        unicode_like,
        0,
        0
    );
}




void unicode_like(sqlite3_context *ctx, int n, sqlite3_value **args)
{
    Q_ASSERT(n == 2);

    UErrorCode err = U_ZERO_ERROR;

    UCollator *collator = (UCollator*)sqlite3_user_data(ctx);

    UStringSearch *search = usearch_openFromCollator(
        (UChar*)sqlite3_value_text16(args[0]),
        -1,
        (UChar*)sqlite3_value_text16(args[1]),
        -1,
        collator,
        0,
        &err
    );

    int result = 0;

    int pos = usearch_first(search, &err);
    if (pos != -1)
        result = 1;

    usearch_close(search);

    sqlite3_result_int(ctx, result);
}
