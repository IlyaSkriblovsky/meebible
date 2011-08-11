#include "Cache.h"

#include <QDebug>
#include <QDesktopServices>

#include <QSqlQuery>
#include <QVariant>

#include "Paths.h"
#include "Translation.h"
#include "Language.h"

#include "SqliteUnicodeSearch.h"

#include "SearchThread.h"


Cache* Cache::_instance = 0;


Cache* Cache::instance()
{
    if (_instance)
        return _instance;

    return new Cache;
}


Cache::Cache()
{
    if (_instance)
        qDebug() << "Duplicate Cache instance";
    _instance = this;


    _db = QSqlDatabase::addDatabase("QSQLITE", "cache");
    _db.setDatabaseName(Paths::cacheDB());
    if (! _db.open())
        qDebug() << "Cannot open cache DB";

    SqliteUnicodeSearch::installUnicodeSearch(_db);

    _db.exec(
        "CREATE TABLE IF NOT EXISTS html ( "
            "transCode VARCHAR, "
            "langCode VARCHAR, "
            "bookCode VARCHAR, "
            "bookNo INTEGER, "
            "chapterNo INTEGER, "
            "html, "
            "text, "
            "PRIMARY KEY (transCode, langCode, bookCode, chapterNo)"
        ")"
    );
    _db.exec(
        "CREATE INDEX IF NOT EXISTS lc_tc_bn_cn ON html "
        "(langCode, transCode, bookNo, chapterNo)"
    );


    _stripTags = QRegExp("<.*>");
    _stripTags.setMinimal(true);
    _stripSpaces = QRegExp("\\s+");
}


Cache::~Cache()
{
}


void Cache::saveChapter(const Translation* translation, const QString& bookCode, int chapterNo, QString html)
{
    QSqlQuery insert(_db);
    insert.prepare("REPLACE INTO html VALUES (:transCode, :langCode, :bookCode, :bookNo, :chapterNo, :html, :text)");
    insert.bindValue(":transCode", translation->code());
    insert.bindValue(":langCode", translation->language()->code());
    insert.bindValue(":bookCode", bookCode);
    insert.bindValue(":bookNo", translation->bookCodes().indexOf(bookCode));
    insert.bindValue(":chapterNo", chapterNo);
    insert.bindValue(":html", html);

    QString text = html;
    text.replace(_stripTags, " ");
    text.replace(_stripSpaces, " ");
    insert.bindValue(":text", text);

    if (! insert.exec())
        qDebug() << "Insertion into cache failed";
}


QString Cache::loadChapter(const Translation *translation, const QString& bookCode, int chapterNo)
{
    QSqlQuery select(_db);
    select.prepare("SELECT html FROM html WHERE transCode=:transCode AND langCode=:langCode AND bookCode=:bookCode AND chapterNo=:chapterNo");
    select.bindValue(":transCode", translation->code());
    select.bindValue(":langCode", translation->language()->code());
    select.bindValue(":bookCode", bookCode);
    select.bindValue(":chapterNo", chapterNo);
    if (! select.exec())
        qDebug() << "Selection from cache failed";

    if (select.next())
        return select.value(0).toString();
    else
        return QString();
}


bool Cache::hasChapter(const Translation* translation, const QString& bookCode, int chapterNo)
{
    QSqlQuery select(_db);
    select.prepare("SELECT count(*) FROM html WHERE transCode=:transCode AND langCode=:langCode AND bookCode=:bookCode AND chapterNo=:chapterNo");
    select.bindValue(":transCode", translation->code());
    select.bindValue(":langCode", translation->language()->code());
    select.bindValue(":bookCode", bookCode);
    select.bindValue(":chapterNo", chapterNo);
    if (! select.exec())
        qDebug() << "Selection from cache failed";

    select.next();
    return select.value(0).toBool();
}


int Cache::totalChaptersInCache(const Translation *translation)
{
    QSqlQuery select(_db);
    select.prepare("SELECT count(*) FROM html WHERE langCode=:langCode AND transCode=:transCode");
    select.addBindValue(translation->language()->code());
    select.addBindValue(translation->code());
    select.exec();
    select.next();

    return select.value(0).toInt();
}



void Cache::search(Translation* translation, const QString& text)
{
    searchStarted();

    SearchThread* thread = new SearchThread(translation, text);
    connect(thread, SIGNAL(matchFound(QString, int, QString, int)), this, SLOT(onThreadMatchFound(QString, int, QString, int)));
    connect(thread, SIGNAL(finished()), this, SLOT(onThreadFinished()));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

    thread->start();
}

void Cache::onThreadMatchFound(const QString& bookCode, int chapterNo, QString match, int matchCount)
{
    matchFound(bookCode, chapterNo, match, matchCount);
}

void Cache::onThreadFinished()
{
    searchFinished();
}
