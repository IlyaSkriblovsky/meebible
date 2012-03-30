#include "Cache.h"

#include <QDebug>
#include <QDesktopServices>

#include <QVariant>

#include "sqlite3/sqlite3.h"

#include "Paths.h"
#include "Translation.h"
#include "Language.h"

#ifndef NOSEARCH
    #include "SqliteUnicodeSearch.h"
    #include "SearchThread.h"
#endif

#include "unisimple/tokenizer.h"



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
    if (sqlite3_open(Paths::cacheDB().toUtf8(), &_db) != SQLITE_OK)
        qCritical() << "Cannot open cache DB";

    #ifndef NOSEARCH
        SqliteUnicodeSearch::installUnicodeSearch(_db);
    #endif

    install_unisimple_tokenizer(_db);

    sqlite3_exec(_db,
        "CREATE TABLE IF NOT EXISTS html ( "
            "transCode VARCHAR, "
            "langCode VARCHAR, "
            "bookCode VARCHAR, "
            "bookNo INTEGER, "
            "chapterNo INTEGER, "
            "html, "
            "text, "
            "PRIMARY KEY (transCode, langCode, bookCode, chapterNo)"
        ")",
        0, 0, 0
    );
    sqlite3_exec(_db,
        "CREATE INDEX IF NOT EXISTS lc_tc_bn_cn ON html "
        "(langCode, transCode, bookNo, chapterNo)",
        0, 0, 0
    );
    sqlite3_exec(_db,
        "CREATE VIRTUAL TABLE IF NOT EXISTS text USING fts4(tokenize=unisimple)",
        0, 0, 0
    );

    sqlite3_prepare_v2(_db,
        "REPLACE INTO html VALUES (:transCode, :langCode, :bookCode, :bookNo, :chapterNo, :html, :text)", -1,
        &_stmt_saveChapter, 0
    );

    sqlite3_prepare_v2(_db,
        "SELECT html FROM html WHERE transCode=:transCode AND langCode=:langCode AND bookCode=:bookCode AND chapterNo=:chapterNo", -1,
        &_stmt_loadChapter, 0
    );

    sqlite3_prepare_v2(_db,
        "SELECT count(*) FROM html WHERE transCode=:transCode AND langCode=:langCode AND bookCode=:bookCode AND chapterNo=:chapterNo", -1,
        &_stmt_hasChapter, 0
    );

    sqlite3_prepare_v2(_db,
        "SELECT count(*) FROM html WHERE langCode=:langCode AND transCode=:transCode", -1,
        &_stmt_totalChapters, 0
    );


    sqlite3_prepare_v2(_db,
        "INSERT INTO text (content) VALUES (?)", -1,
        &_stmt_saveChapter_fts, 0
    );
}


Cache::~Cache()
{
    if (_db)
        sqlite3_close(_db);
}


void Cache::saveChapter(const Translation* translation, const Place& place, QString html)
{
    sqlite3_reset(_stmt_saveChapter);
    sqlite3_bind_text16(_stmt_saveChapter, 1, translation->code().utf16(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text16(_stmt_saveChapter, 2, translation->language()->code().utf16(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text16(_stmt_saveChapter, 3, place.bookCode().utf16(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int   (_stmt_saveChapter, 4, translation->bookCodes().indexOf(place.bookCode()));
    sqlite3_bind_int   (_stmt_saveChapter, 5, place.chapterNo());
    sqlite3_bind_text16(_stmt_saveChapter, 6, html.utf16(), -1, SQLITE_STATIC);

    QString text = html;
    text.replace(_stripStyles, " ");
    text.replace(_stripTags, " ");
    text.replace(_stripSpaces, " ");
    sqlite3_bind_text16(_stmt_saveChapter, 7, text.utf16(), -1, SQLITE_STATIC);

    if (sqlite3_step(_stmt_saveChapter) != SQLITE_DONE)
        qCritical() << "Insertion into cache failed:" << sqlite3_errmsg(_db);


    sqlite3_reset(_stmt_saveChapter_fts);
    sqlite3_bind_text16(_stmt_saveChapter_fts, 1, html.utf16(), -1, SQLITE_STATIC);
    if (sqlite3_step(_stmt_saveChapter_fts) != SQLITE_DONE)
        qCritical() << "Insertin into text cache failed:" << sqlite3_errmsg(_db);
}


QString Cache::loadChapter(const Translation *translation, const Place& place)
{
    sqlite3_reset(_stmt_loadChapter);
    sqlite3_bind_text16(_stmt_loadChapter, 1, translation->code().utf16(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text16(_stmt_loadChapter, 2, translation->language()->code().utf16(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text16(_stmt_loadChapter, 3, place.bookCode().utf16(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int   (_stmt_loadChapter, 4, place.chapterNo());

    if (sqlite3_step(_stmt_loadChapter) == SQLITE_ROW)
        return QString::fromUtf16((const ushort*)sqlite3_column_text16(_stmt_loadChapter, 0));
    else
        return QString();
}


bool Cache::hasChapter(const Translation* translation, const Place& place)
{
    sqlite3_reset(_stmt_hasChapter);
    sqlite3_bind_text16(_stmt_hasChapter, 1, translation->code().utf16(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text16(_stmt_hasChapter, 2, translation->language()->code().utf16(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text16(_stmt_hasChapter, 3, place.bookCode().utf16(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int   (_stmt_hasChapter, 4, place.chapterNo());

    if (sqlite3_step(_stmt_hasChapter) == SQLITE_ROW)
        return sqlite3_column_int(_stmt_hasChapter, 0) != 0;
    else
        return false;
}


int Cache::totalChaptersInCache(const Translation *translation)
{
    sqlite3_reset(_stmt_totalChapters);
    sqlite3_bind_text16(_stmt_totalChapters, 1, translation->language()->code().utf16(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text16(_stmt_totalChapters, 2, translation->code().utf16(), -1, SQLITE_TRANSIENT);
    if (sqlite3_step(_stmt_totalChapters) == SQLITE_ROW)
        return sqlite3_column_int(_stmt_totalChapters, 0);
    else
        return 0;
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
    sqlite3_close(_db); _db = 0;

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
