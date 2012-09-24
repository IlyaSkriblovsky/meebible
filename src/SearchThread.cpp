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
    Indexer* indexer,
    const Translation* translation, const QString& query,
    int maxResults,
    QObject* parent/* = 0 */
)
    : QThread(parent), _indexer(indexer),
     _translation(translation), _query(query),
     _maxResults(maxResults)
{
}

typedef QPair<int, int> ChapterId;

void SearchThread::run()
{
    qDebug() << "Search thread started";

    QElapsedTimer timer; timer.start();


    QList<SearchQueryParser::QueryToken> queryTokens = SearchQueryParser::parseQuery(_query);

    QMap<ChapterId, QList<MatchEntry> > results = _indexer->search(_query, 10);

    // QString sql = QString("SELECT bookCode, chapterNo, substr(text, ?, 50) FROM html WHERE transCode=? AND langCode=? AND bookNo=? AND chapterNo=? LIMIT 1");

    // sqlite3_stmt* stmt;
    // sqlite3_prepare16_v2(_db, sql.utf16(), -1, &stmt, 0);

    foreach (const ChapterId& chapterId, results.keys())
    {
        const QList<MatchEntry>& entries = results.value(chapterId);

        SearchMatch match;
        match.bookNo = chapterId.first;
        match.chapterNo = chapterId.second;
        match.offset = entries[0].offset;
        match.matchesInChapter = entries.size();

        _results.append(match);

        // sqlite3_reset(stmt);

        // sqlite3_bind_int(stmt, 1, entries[0].offset < 15 ? 0 : entries[0].offset - 15);
        // sqlite3_bind_text16(stmt, 2, _translation->code().utf16(), -1, SQLITE_STATIC);
        // sqlite3_bind_text16(stmt, 3, _translation->language()->code().utf16(), -1, SQLITE_STATIC);
        // sqlite3_bind_int(stmt, 4, chapterId.first);
        // sqlite3_bind_int(stmt, 5, chapterId.second);

        // if (sqlite3_step(stmt) == SQLITE_ROW)
        // {
        //     QString textChunk = QString::fromUtf16((const ushort*)sqlite3_column_text16(stmt, 2));

        //     searchResults.append(QVariant::fromValue(SearchResult(
        //         Place(
        //             QString::fromUtf16((const ushort*)sqlite3_column_text16(stmt, 0)),
        //             sqlite3_column_int(stmt, 1)
        //         ),
        //         Highlighter::highlight(textChunk, queryTokens, "<u>", "</u>", -1),
        //         entries.size()
        //     )));
        // }

        if (_results.size() >= _maxResults)
            break;
    }

    // sqlite3_finalize(stmt);

    qDebug() << "search elapsed:" << timer.elapsed();
    finished();

    qDebug() << "Search thread stopped";
}
