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
    qDebug() << "~~~ SearchThread::~SearchThread()";
}

#include <QElapsedTimer>

void SearchThread::run()
{
    QElapsedTimer timer;
    timer.start();

    QSqlQuery select(_db);
    select.prepare(
        "SELECT bookCode, chapterNo "
        "FROM html "
        "WHERE langCode=:langCode AND transCode=:transCode AND html LIKE :needle "
        "ORDER BY bookNo, chapterNo"
    );
    select.addBindValue(_langCode);
    select.addBindValue(_transCode);
    select.addBindValue(_needle);
    if (! select.exec())
        qDebug() << select.lastError().text();

    qDebug() << "start";

    while (select.next())
        matchFound(select.value(0).toString(), select.value(1).toInt());

    qDebug() << "done";

    qDebug() << "elapsed:" << timer.elapsed();


/*
    QElapsedTimer timer;
    timer.start();

    _db.exec("DROP TABLE IF EXISTS sorted");
    _db.exec("CREATE TEMPORARY TABLE sorted (id INTEGER PRIMARY KEY, bookCode, chapterNo, html)");
    QSqlQuery createSorted(_db);
    createSorted.prepare("INSERT INTO sorted SELECT NULL, bookCode, chapterNo, html FROM html WHERE langCode=:langCode AND transCode=:transCode ORDER BY bookNo");
    createSorted.addBindValue(_langCode);
    createSorted.addBindValue(_transCode);
    createSorted.exec();

    qDebug() << "creating table:" << timer.elapsed();

    QSqlQuery select(_db);
    select.prepare(
        "SELECT bookCode, chapterNo "
        "FROM sorted "
        "WHERE html LIKE :needle "
    );
    select.addBindValue(_needle);
    if (! select.exec())
        qDebug() << select.lastError().text();

    qDebug() << "start";

    while (select.next())
        matchFound(select.value(0).toString(), select.value(1).toInt());

    _db.exec("DROP TABLE sorted");

    qDebug() << "done";

    qDebug() << "elapsed:" << timer.elapsed();
*/
}
