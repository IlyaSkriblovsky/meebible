#ifndef SQLITEUNICODESEARCH_H
#define SQLITEUNICODESEARCH_H

#include <QString>
#include <QSqlDatabase>


namespace SqliteUnicodeSearch
{

    void installUnicodeSearch(const QSqlDatabase& db);

    QString highlightMatches(const QString& html, const QString& needle);
}

#endif
