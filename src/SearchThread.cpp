#include "SearchThread.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

#include "Translation.h"
#include "Language.h"


SearchThread::SearchThread(const Translation* translation, const QString& needle, QObject* parent):
    QThread(parent), _needle(needle)
{
    _langCode = translation->language()->code();
    _transCode = translation->code();

    _db = QSqlDatabase::database("cache");
}

SearchThread::~SearchThread()
{
}

#include <QElapsedTimer>

void SearchThread::run()
{
    QElapsedTimer timer;
    timer.start();

    QSqlQuery select(_db);
    select.prepare(
        "SELECT bookCode, chapterNo, unicodeMatch(:needle, text) as match, matchCount() "
        "FROM html "
        "WHERE langCode=:langCode AND transCode=:transCode AND match IS NOT NULL "
        "ORDER BY bookNo, chapterNo"
    );
    select.addBindValue(_needle);
    select.addBindValue(_langCode);
    select.addBindValue(_transCode);
    if (! select.exec())
        qDebug() << select.lastError().text();

    while (select.next())
    {
        matchFound(
            Place(
                select.value(0).toString(),
                select.value(1).toInt()
            ),
            select.value(2).toString(),
            select.value(3).toInt()
        );
    }
}
