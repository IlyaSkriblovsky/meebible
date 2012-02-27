#include "Cache.h"

#include <QDebug>
#include <QDesktopServices>

#include <QVariant>

#include "Paths.h"
#include "Translation.h"
#include "Language.h"

#ifndef NOSEARCH
    #include "SqliteUnicodeSearch.h"
    #include "SearchThread.h"
#endif



Cache* Cache::_instance = 0;


Cache* Cache::instance()
{
    if (_instance)
        return _instance;

    return new Cache;
}


Cache::Cache()
    #ifndef NOSEARCH
        : _searchInProgress(false)
    #endif
{
    if (_instance)
        qDebug() << "Duplicate Cache instance";
    _instance = this;


    openDB();


    _stripTags = QRegExp("<.*>");
    _stripTags.setMinimal(true);
    _stripSpaces = QRegExp("\\s+");
    _stripStyles = QRegExp("<style>.*</style>");
    _stripStyles.setMinimal(true);
}


void Cache::openDB()
{
    _db = QSqlDatabase::addDatabase("QSQLITE", "cache");
    _db.setDatabaseName(Paths::cacheDB());
    if (! _db.open())
        qDebug() << "Cannot open cache DB";

    #ifndef NOSEARCH
        SqliteUnicodeSearch::installUnicodeSearch(_db);
    #endif

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

    _stmt_saveChapter = QSqlQuery(_db);
    _stmt_saveChapter.prepare("REPLACE INTO html VALUES (:transCode, :langCode, :bookCode, :bookNo, :chapterNo, :html, :text)");

    _stmt_loadChapter = QSqlQuery(_db);
    _stmt_loadChapter.prepare("SELECT html FROM html WHERE transCode=:transCode AND langCode=:langCode AND bookCode=:bookCode AND chapterNo=:chapterNo");

    _stmt_hasChapter = QSqlQuery(_db);
    _stmt_hasChapter.prepare("SELECT count(*) FROM html WHERE transCode=:transCode AND langCode=:langCode AND bookCode=:bookCode AND chapterNo=:chapterNo");

    _stmt_totalChapters = QSqlQuery(_db);
    _stmt_totalChapters.prepare("SELECT count(*) FROM html WHERE langCode=:langCode AND transCode=:transCode");
}


Cache::~Cache()
{
}


void Cache::saveChapter(const Translation* translation, const Place& place, QString html)
{
    _stmt_saveChapter.bindValue(":transCode", translation->code());
    _stmt_saveChapter.bindValue(":langCode", translation->language()->code());
    _stmt_saveChapter.bindValue(":bookCode", place.bookCode());
    _stmt_saveChapter.bindValue(":bookNo", translation->bookCodes().indexOf(place.bookCode()));
    _stmt_saveChapter.bindValue(":chapterNo", place.chapterNo());
    _stmt_saveChapter.bindValue(":html", html);

    QString text = html;
    text.replace(_stripStyles, " ");
    text.replace(_stripTags, " ");
    text.replace(_stripSpaces, " ");
    _stmt_saveChapter.bindValue(":text", text);

    if (! _stmt_saveChapter.exec())
        qDebug() << "Insertion into cache failed";
}


QString Cache::loadChapter(const Translation *translation, const Place& place)
{
    _stmt_loadChapter.bindValue(":transCode", translation->code());
    _stmt_loadChapter.bindValue(":langCode", translation->language()->code());
    _stmt_loadChapter.bindValue(":bookCode", place.bookCode());
    _stmt_loadChapter.bindValue(":chapterNo", place.chapterNo());
    if (! _stmt_loadChapter.exec())
        qDebug() << "Selection from cache failed";

    if (_stmt_loadChapter.next())
        return _stmt_loadChapter.value(0).toString();
    else
        return QString();
}


bool Cache::hasChapter(const Translation* translation, const Place& place)
{
    _stmt_hasChapter.bindValue(":transCode", translation->code());
    _stmt_hasChapter.bindValue(":langCode", translation->language()->code());
    _stmt_hasChapter.bindValue(":bookCode", place.bookCode());
    _stmt_hasChapter.bindValue(":chapterNo", place.chapterNo());
    if (! _stmt_hasChapter.exec())
        qDebug() << "Selection from cache failed";

    _stmt_hasChapter.next();
    return _stmt_hasChapter.value(0).toBool();
}


int Cache::totalChaptersInCache(const Translation *translation)
{
    _stmt_totalChapters.addBindValue(translation->language()->code());
    _stmt_totalChapters.addBindValue(translation->code());
    _stmt_totalChapters.exec();
    _stmt_totalChapters.next();

    return _stmt_totalChapters.value(0).toInt();
}



#ifndef NOSEARCH
void Cache::search(Translation* translation, const QString& text)
{
    if (_searchInProgress)
        return;

    _searchInProgress = true;
    searchInProgressChanged();

    searchStarted();

    SearchThread* thread = new SearchThread(translation, text);
    connect(thread, SIGNAL(matchFound(Place, QString, int)), this, SLOT(onThreadMatchFound(Place, QString, int)));
    connect(thread, SIGNAL(finished()), this, SLOT(onThreadFinished()));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

    thread->start();
}

void Cache::onThreadMatchFound(Place place, QString match, int matchCount)
{
    matchFound(place, match, matchCount);
}

void Cache::onThreadFinished()
{
    _searchInProgress = false;
    searchInProgressChanged();

    searchFinished();
}
#endif



void Cache::clearCache()
{
    _db.close();
    QSqlDatabase::removeDatabase("cache");

    QFile cache_db(Paths::cacheDB());
    if (! cache_db.remove())
        qDebug() << "Cannot remove cache db!";

    openDB();

    QStringList xml = Paths::allCachedXML("trans_");
    for (int i = 0; i < xml.size(); i++)
    {
        qDebug() << "Deleting" << Paths::cachedXML(xml.at(i));
        QFile::remove(Paths::cachedXML(xml.at(i)));
    }
}




void Cache::saveXML(const QString& name, const QString& content)
{
    QString filename = Paths::cachedXML(name);
    QFile xml(filename);
    if (! xml.open(QIODevice::WriteOnly))
        qWarning() << "Cannot save cache xml:" << filename;
    else
        xml.write(content.toUtf8());
}


bool Cache::hasXML(const QString& name)
{
    return QFile::exists(Paths::cachedXML(name));
}


QString Cache::loadXML(const QString& name)
{
    if (! hasXML(name)) return QString();

    QString filename = Paths::cachedXML(name);
    QFile xml(filename);
    if (! xml.open(QIODevice::ReadOnly))
    {
        qWarning() << "Cannot load cache xml:" << filename;
        return QString();
    }

    return QString::fromUtf8(xml.readAll());
}
