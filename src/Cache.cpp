#include "Cache.h"

#include <QDebug>
#include <QDesktopServices>

#include <QVariant>
#include <QElapsedTimer>
#include <QThread>

#include <sqlite3.h>

#ifdef ASYNC_DB_IO
    #include "3rdparty/sqlite3async/sqlite3async.h"
#endif

#include "Paths.h"
#include "Translation.h"
#include "Language.h"

#ifndef NOSEARCH
    #include "SqliteUnicodeSearch.h"
    #include "SearchThread.h"
#endif



#ifdef ASYNC_DB_IO

class SqliteAsyncThread: public QThread
{
    public:
        SqliteAsyncThread(QObject* parent = 0): QThread(parent)
        {
        }

        void run()
        {
            qDebug() << "Async I/O thread started";
            sqlite3async_run();
            qDebug() << "Async I/O thread stopped";
        }
};

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

    #ifdef ASYNC_DB_IO
        sqlite3async_initialize(0, 1);
    #endif

    openDB();


    _stripTags = QRegExp("<.*>");
    _stripTags.setMinimal(true);
    _stripSpaces = QRegExp("\\s+");
    _stripStyles = QRegExp("<style>.*</style>");
    _stripStyles.setMinimal(true);
}

Cache::~Cache()
{
    closeDB();
}

void Cache::execWithCheck(const char* sql)
{
    char *errmsg;
    sqlite3_exec(_db, sql, 0, 0, &errmsg);
    if (errmsg)
    {
        qCritical() << "SQL error:" << errmsg;
        sqlite3_free(errmsg);
    }
}


void Cache::openDB()
{
    #ifdef ASYNC_DB_IO
        sqlite3async_control(SQLITEASYNC_HALT, SQLITEASYNC_HALT_NEVER);
        _asyncThread = new SqliteAsyncThread(this);
        _asyncThread->start();
    #endif


    if (sqlite3_open_v2(Paths::cacheDB().toUtf8(), &_db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, "sqlite3async") != SQLITE_OK)
        qCritical() << "Cannot open cache DB";

    #ifndef NOSEARCH
        SqliteUnicodeSearch::installUnicodeSearch(_db);
    #endif

    execWithCheck("PRAGMA synchronous = 0");

    execWithCheck(
        "CREATE TABLE IF NOT EXISTS html ( "
            "transCode VARCHAR, "
            "langCode VARCHAR, "
            "bookCode VARCHAR, "
            "bookNo INTEGER, "
            "chapterNo INTEGER, "
            "html TEXT, "
            "text TEXT, "
            "PRIMARY KEY (transCode, langCode, bookCode, chapterNo)"
        ")"
    );
    execWithCheck(
        "CREATE INDEX IF NOT EXISTS lc_tc_bn_cn ON html "
        "(langCode, transCode, bookNo, chapterNo)"
    );

    sqlite3_prepare_v2(_db,
        "INSERT OR REPLACE INTO html "
            "(transCode, langCode, bookCode, bookNo, chapterNo, html, text) "
            "VALUES "
            "(?, ?, ?, ?, ?, ?, ?);",
        -1,
        &_stmt_saveChapter, 0
    );

    sqlite3_prepare_v2(_db,
        "SELECT html FROM html WHERE transCode=? AND langCode=? AND bookCode=? AND chapterNo=?",
        -1,
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
        "SELECT bookCode, chapterNo FROM html WHERE langCode=? AND transCode=? ORDER BY bookNo, chapterNo", -1,
        &_stmt_availableChapters, 0
    );
}

void Cache::closeDB()
{
    if (_db == 0) return;

    sqlite3_finalize(_stmt_saveChapter); _stmt_saveChapter = 0;
    sqlite3_finalize(_stmt_loadChapter); _stmt_loadChapter = 0;
    sqlite3_finalize(_stmt_hasChapter); _stmt_hasChapter = 0;
    sqlite3_finalize(_stmt_totalChapters); _stmt_totalChapters = 0;
    sqlite3_finalize(_stmt_availableChapters); _stmt_availableChapters = 0;

    #ifdef ASYNC_DB_IO
        sqlite3async_control(SQLITEASYNC_HALT, SQLITEASYNC_HALT_IDLE);
        _asyncThread->wait();
        delete _asyncThread;
        _asyncThread = 0;
    #endif


    if (sqlite3_close(_db) != SQLITE_OK)
        qCritical() << "Error while closing DB:" << sqlite3_errmsg(_db);
    _db = 0;
}



void Cache::saveChapter(const Translation* translation, const Place& place, QString html)
{
    QString transCode = translation->code();
    QString langCode = translation->language()->code();
    QString bookCode = place.bookCode();

    sqlite3_reset(_stmt_saveChapter);
    sqlite3_bind_text16(_stmt_saveChapter, 1, transCode.utf16(), -1, SQLITE_STATIC);
    sqlite3_bind_text16(_stmt_saveChapter, 2, langCode.utf16(), -1, SQLITE_STATIC);
    sqlite3_bind_text16(_stmt_saveChapter, 3, bookCode.utf16(), -1, SQLITE_STATIC);
    sqlite3_bind_int   (_stmt_saveChapter, 4, translation->bookCodes().indexOf(place.bookCode()));
    sqlite3_bind_int   (_stmt_saveChapter, 5, place.chapterNo());
    sqlite3_bind_text16(_stmt_saveChapter, 6, html.utf16(), -1, SQLITE_STATIC);

    QString text = html;
    text.replace(_stripStyles, " ");
    text.replace(_stripTags, " ");
    text.replace(_stripSpaces, " ");
    sqlite3_bind_text16(_stmt_saveChapter, 7, text.utf16(), -1, SQLITE_STATIC);


    if (sqlite3_step(_stmt_saveChapter) != SQLITE_DONE)
        qCritical() << "SQL error in saveChapter:" << sqlite3_errmsg(_db);
}


QString Cache::loadChapter(const Translation *translation, const Place& place)
{
    QString transCode = translation->code();
    QString langCode = translation->language()->code();
    QString bookCode = place.bookCode();

    sqlite3_reset(_stmt_loadChapter);
    sqlite3_bind_text16(_stmt_loadChapter, 1, transCode.utf16(), -1, SQLITE_STATIC);
    sqlite3_bind_text16(_stmt_loadChapter, 2, langCode.utf16(), -1, SQLITE_STATIC);
    sqlite3_bind_text16(_stmt_loadChapter, 3, bookCode.utf16(), -1, SQLITE_STATIC);
    sqlite3_bind_int   (_stmt_loadChapter, 4, place.chapterNo());

    int result = sqlite3_step(_stmt_loadChapter);

    if (result == SQLITE_ROW)
        return QString::fromUtf16((const ushort*)sqlite3_column_text16(_stmt_loadChapter, 0));
    else if (result == SQLITE_DONE)
        return QString();
    else
    {
        qCritical() << "SQL error in loadChapter:" << sqlite3_errmsg(_db);
        return QString();
    }
}


bool Cache::hasChapter(const Translation* translation, const Place& place)
{
    QString transCode = translation->code();
    QString langCode = translation->language()->code();
    QString bookCode = place.bookCode();

    sqlite3_reset(_stmt_hasChapter);
    sqlite3_bind_text16(_stmt_hasChapter, 1, transCode.utf16(), -1, SQLITE_STATIC);
    sqlite3_bind_text16(_stmt_hasChapter, 2, langCode.utf16(), -1, SQLITE_STATIC);
    sqlite3_bind_text16(_stmt_hasChapter, 3, bookCode.utf16(), -1, SQLITE_STATIC);
    sqlite3_bind_int   (_stmt_hasChapter, 4, place.chapterNo());

    int result = sqlite3_step(_stmt_hasChapter);

    if (result == SQLITE_ROW)
        return sqlite3_column_int(_stmt_hasChapter, 0) != 0;
    else if (result == SQLITE_DONE)
        return false;
    else
    {
        qCritical() << "SQL error in hasChapter:" << sqlite3_errmsg(_db);
        return false;
    }
}


int Cache::totalChaptersInCache(const Translation *translation)
{
    QString transCode = translation->code();
    QString langCode = translation->language()->code();

    sqlite3_reset(_stmt_totalChapters);
    sqlite3_bind_text16(_stmt_totalChapters, 1, langCode.utf16(), -1, SQLITE_STATIC);
    sqlite3_bind_text16(_stmt_totalChapters, 2, transCode.utf16(), -1, SQLITE_STATIC);

    int result = sqlite3_step(_stmt_totalChapters);

    if (result == SQLITE_ROW)
        return sqlite3_column_int(_stmt_totalChapters, 0);
    else if (result == SQLITE_DONE)
        return 0;
    else
    {
        qCritical() << "SQL error in totalChaptersInCache:" << sqlite3_errmsg(_db);
        return 0;
    }
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
    closeDB();

    QFile cache_db(Paths::cacheDB());
    if (! cache_db.remove())
        qDebug() << "Cannot remove cache db!";
    else
        qDebug() << "Cache cleared";

    qDebug() << QFile("/home/user/.cache/MeeBible/MeeBible/cache.sqlite-journal").exists();

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


QSet<QPair<QString, int> > Cache::availableChapters(const Translation* translation)
{
    QString transCode = translation->code();
    QString langCode = translation->language()->code();

    sqlite3_reset(_stmt_availableChapters);
    sqlite3_bind_text16(_stmt_availableChapters, 1, langCode.utf16(), -1, SQLITE_STATIC);
    sqlite3_bind_text16(_stmt_availableChapters, 2, transCode.utf16(), -1, SQLITE_STATIC);

    QSet<QPair<QString, int> > result;
    while (sqlite3_step(_stmt_availableChapters) == SQLITE_ROW)
        result.insert(QPair<QString, int>(
            QString::fromUtf16((const ushort*)sqlite3_column_text16(_stmt_availableChapters, 0)),
            sqlite3_column_int(_stmt_availableChapters, 1)
        ));

    return result;
}


void Cache::beginTransaction()
{
    sqlite3_exec(_db, "BEGIN", 0, 0, 0);
}

void Cache::commitTransaction()
{
    QElapsedTimer timer; timer.start();
    sqlite3_exec(_db, "COMMIT", 0, 0, 0);
    qDebug() << "commit time:" << timer.elapsed();
}
