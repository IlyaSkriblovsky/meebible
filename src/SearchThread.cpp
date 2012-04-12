#include "SearchThread.h"

#include <sqlite3.h>

#include <QDebug>
#include <QElapsedTimer>

#include "Translation.h"
#include "Language.h"
#include "Indexer.h"
#include "SearchQueryParser.h"
#include "Highlighter.h"
#include "SearchResult.h"


SearchThread::SearchThread(
    sqlite3* db, Indexer* indexer,
    bool rebuild,
    const Translation* translation, const QString& query,
    int maxResults,
    QObject* parent/* = 0 */
)
    : QThread(parent), _db(db), _indexer(indexer),
     _rebuild(rebuild),
     _translation(translation), _query(query),
     _maxResults(maxResults)
{
}

typedef QPair<int, int> ChapterId;

void SearchThread::run()
{
    qDebug() << "Search thread started";

    QElapsedTimer timer; timer.start();


    if (_rebuild)
    {
        qDebug() << "Rebuilding index";

        _indexer->clear();
        sqlite3_stmt* select;
        sqlite3_prepare_v2(_db,
            "SELECT bookNo, chapterNo, text FROM html WHERE transCode=? AND langCode=?", -1,
            &select, 0
        );
        sqlite3_bind_text16(select, 1, _translation->code().utf16(), -1, SQLITE_STATIC);
        sqlite3_bind_text16(select, 2, _translation->language()->code().utf16(), -1, SQLITE_STATIC);

        while (sqlite3_step(select) == SQLITE_ROW)
            _indexer->addChapter(
                sqlite3_column_int(select, 0),
                sqlite3_column_int(select, 1),
                QString::fromUtf16((const ushort*)sqlite3_column_text16(select, 2))
            );

        sqlite3_finalize(select);
        indexRebuilt();
    }


    QList<SearchQueryParser::QueryToken> queryTokens = SearchQueryParser::parseQuery(_query);

    QMap<ChapterId, QList<MatchEntry> > results = _indexer->search(_query, 10);

    QString sql = QString("SELECT bookCode, chapterNo, substr(text, ?, 50) FROM html WHERE transCode=? AND langCode=? AND bookNo=? AND chapterNo=? LIMIT 1");

    sqlite3_stmt* stmt;
    sqlite3_prepare16_v2(_db, sql.utf16(), -1, &stmt, 0);

    QList<QVariant> searchResults;

    foreach (const ChapterId& chapterId, results.keys())
    {
        const QList<MatchEntry>& entries = results.value(chapterId);

        sqlite3_reset(stmt);

        sqlite3_bind_int(stmt, 1, entries[0].offset < 15 ? 0 : entries[0].offset - 15);
        sqlite3_bind_text16(stmt, 2, _translation->code().utf16(), -1, SQLITE_STATIC);
        sqlite3_bind_text16(stmt, 3, _translation->language()->code().utf16(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 4, chapterId.first);
        sqlite3_bind_int(stmt, 5, chapterId.second);

        if (sqlite3_step(stmt) == SQLITE_ROW)
        {
            QString textChunk = QString::fromUtf16((const ushort*)sqlite3_column_text16(stmt, 2));

            searchResults.append(QVariant::fromValue(SearchResult(
                Place(
                    QString::fromUtf16((const ushort*)sqlite3_column_text16(stmt, 0)),
                    sqlite3_column_int(stmt, 1)
                ),
                Highlighter::highlight(textChunk, queryTokens, "<u>", "</u>", -1),
                entries.size()
            )));
        }

        if (searchResults.size() == _maxResults)
            break;
    }

    sqlite3_finalize(stmt);

    qDebug() << "search elapsed:" << timer.elapsed();
    finished(searchResults);

    qDebug() << "Search thread stopped";
}
