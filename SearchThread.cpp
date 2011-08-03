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
}
