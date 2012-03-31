#include "Cache.h"

#include <QDebug>
#include <QDesktopServices>

#include <QVariant>
#include <QElapsedTimer>
#include <QThread>

#include "sqlite3/sqlite3.h"
#include "sqlite3/sqlite3async.h"

#include "Paths.h"
#include "Translation.h"
#include "Language.h"

#ifndef NOSEARCH
    #include "SqliteUnicodeSearch.h"
    #include "SearchThread.h"
#endif

#include "unisimple/tokenizer.h"


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


    sqlite3async_initialize(0, 1);

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
    sqlite3async_control(SQLITEASYNC_HALT, SQLITEASYNC_HALT_NEVER);
    _asyncThread = new SqliteAsyncThread(this);
    _asyncThread->start();


    if (sqlite3_open_v2(Paths::cacheDB().toUtf8(), &_db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, "sqlite3async") != SQLITE_OK)
        qCritical() << "Cannot open cache DB";

    #ifndef NOSEARCH
        SqliteUnicodeSearch::installUnicodeSearch(_db);
    #endif

    install_unisimple_tokenizer(_db);

    execWithCheck("PRAGMA synchronous = 0");

    execWithCheck(
        "CREATE TABLE IF NOT EXISTS chapters ( "
            "transCode VARCHAR, "
            "langCode VARCHAR, "
            "bookCode VARCHAR, "
            "bookNo INTEGER, "
            "chapterNo INTEGER, "
            "text_id INTEGER, "
            "PRIMARY KEY (transCode, langCode, bookCode, chapterNo)"
        ")"
    );
    execWithCheck(
        "CREATE INDEX IF NOT EXISTS lc_tc_bn_cn ON chapters "
        "(langCode, transCode, bookNo, chapterNo)"
    );
    execWithCheck(
        "CREATE VIRTUAL TABLE IF NOT EXISTS text USING fts4(content, tokenize=unisimple)"
    );

    sqlite3_prepare_v2(_db,
        "INSERT INTO text (content) VALUES (?);",
        -1,
        &_stmt_saveChapter_1, 0
    );

    sqlite3_prepare_v2(_db,
        "INSERT OR REPLACE INTO chapters "
            "(transCode, langCode, bookCode, bookNo, chapterNo, text_id) "
            "VALUES "
            "(?, ?, ?, ?, ?, last_insert_rowid());",
        -1,
        &_stmt_saveChapter_2, 0
    );

    sqlite3_prepare_v2(_db,
        "SELECT content FROM text WHERE rowid ="
            "(SELECT text_id FROM chapters WHERE transCode=? AND langCode=? AND bookCode=? AND chapterNo=?)",
        -1,
        &_stmt_loadChapter, 0
    );

    sqlite3_prepare_v2(_db,
        "SELECT count(*) FROM chapters WHERE transCode=:transCode AND langCode=:langCode AND bookCode=:bookCode AND chapterNo=:chapterNo", -1,
        &_stmt_hasChapter, 0
    );

    sqlite3_prepare_v2(_db,
        "SELECT count(*) FROM chapters WHERE langCode=:langCode AND transCode=:transCode", -1,
        &_stmt_totalChapters, 0
    );

    sqlite3_prepare_v2(_db,
        "SELECT bookCode, chapterNo FROM chapters WHERE langCode=? AND transCode=? ORDER BY bookNo, chapterNo", -1,
        &_stmt_availableChapters, 0
    );



    sqlite3_prepare_v2(_db,
        "SELECT count(*) FROM text WHERE content MATCH 'Иегова'", -1,
        &_stmt_ftstest, 0
    );
}

void Cache::closeDB()
{
    if (_db == 0) return;

    sqlite3_finalize(_stmt_saveChapter_1); _stmt_saveChapter_1 = 0;
    sqlite3_finalize(_stmt_saveChapter_2); _stmt_saveChapter_2 = 0;
    sqlite3_finalize(_stmt_loadChapter); _stmt_loadChapter = 0;
    sqlite3_finalize(_stmt_hasChapter); _stmt_hasChapter = 0;
    sqlite3_finalize(_stmt_totalChapters); _stmt_totalChapters = 0;
    sqlite3_finalize(_stmt_availableChapters); _stmt_availableChapters = 0;

    sqlite3_finalize(_stmt_ftstest); _stmt_ftstest = 0;

    sqlite3async_control(SQLITEASYNC_HALT, SQLITEASYNC_HALT_IDLE);
    _asyncThread->wait();
    delete _asyncThread;
    _asyncThread = 0;


    if (sqlite3_close(_db) != SQLITE_OK)
        qCritical() << "Error while closing DB:" << sqlite3_errmsg(_db);
    _db = 0;
}



void Cache::saveChapter(const Translation* translation, const Place& place, QString html)
{
    sqlite3_reset(_stmt_saveChapter_1);
    sqlite3_bind_text16(_stmt_saveChapter_1, 1, html.utf16(), -1, SQLITE_STATIC);

    sqlite3_reset(_stmt_saveChapter_2);
    sqlite3_bind_text16(_stmt_saveChapter_2, 1, translation->code().utf16(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text16(_stmt_saveChapter_2, 2, translation->language()->code().utf16(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text16(_stmt_saveChapter_2, 3, place.bookCode().utf16(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int   (_stmt_saveChapter_2, 4, translation->bookCodes().indexOf(place.bookCode()));
    sqlite3_bind_int   (_stmt_saveChapter_2, 5, place.chapterNo());

    sqlite3_exec(_db, "SAVEPOINT saveChapter", 0, 0, 0);
    sqlite3_step(_stmt_saveChapter_1);
    sqlite3_step(_stmt_saveChapter_2);
    sqlite3_exec(_db, "RELEASE saveChapter", 0, 0, 0);
}


QString Cache::loadChapter(const Translation *translation, const Place& place)
{
    sqlite3_reset(_stmt_loadChapter);
    sqlite3_bind_text16(_stmt_loadChapter, 1, translation->code().utf16(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text16(_stmt_loadChapter, 2, translation->language()->code().utf16(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text16(_stmt_loadChapter, 3, place.bookCode().utf16(), -1, SQLITE_TRANSIENT);
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
    sqlite3_reset(_stmt_hasChapter);
    sqlite3_bind_text16(_stmt_hasChapter, 1, translation->code().utf16(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text16(_stmt_hasChapter, 2, translation->language()->code().utf16(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text16(_stmt_hasChapter, 3, place.bookCode().utf16(), -1, SQLITE_TRANSIENT);
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
    sqlite3_reset(_stmt_totalChapters);
    sqlite3_bind_text16(_stmt_totalChapters, 1, translation->language()->code().utf16(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text16(_stmt_totalChapters, 2, translation->code().utf16(), -1, SQLITE_TRANSIENT);

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
    sqlite3_reset(_stmt_availableChapters);
    sqlite3_bind_text16(_stmt_availableChapters, 1, translation->language()->code().utf16(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text16(_stmt_availableChapters, 2, translation->code().utf16(), -1, SQLITE_TRANSIENT);

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



#include <QElapsedTimer>
void Cache::ftstest()
{
    QElapsedTimer timer;
    timer.start();
    sqlite3_reset(_stmt_ftstest);
    if (sqlite3_step(_stmt_ftstest) != SQLITE_ROW)
        qDebug() << "err:" << sqlite3_errmsg(_db);
    qDebug() << "Found: " << sqlite3_column_int(_stmt_ftstest, 0) << "elapsed:" << timer.elapsed();
}
