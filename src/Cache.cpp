#include "Cache.h"

#include <QDebug>
#include <QDesktopServices>
#include <QVariant>

#include <QVariant>
#include <QElapsedTimer>
#include <QThread>

#include <sqlite3.h>

#include "Paths.h"
#include "Translation.h"
#include "Language.h"
#include "SearchThread.h"

#include "IndexRebuildThread.h"

#include "SearchResult.h"
#include "Highlighter.h"
#include "SearchQueryParser.h"




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


    _storage = 0;


    _stripTags = QRegExp("<.*>");
    _stripTags.setMinimal(true);
    _stripSpaces = QRegExp("\\s+");
    _stripStyles = QRegExp("<style>.*</style>");
    _stripStyles.setMinimal(true);
}

Cache::~Cache()
{
    delete _storage;
}



void Cache::syncIndex()
{
    qDebug() << "syncIndex";
    _indexer.sync();
}


// void Cache::clearCache()
// {
//     closeDB();
// 
//     QFile cache_db(Paths::cacheDB());
//     if (! cache_db.remove())
//         qDebug() << "Cannot remove cache db!";
//     else
//         qDebug() << "Cache cleared";
// 
//     qDebug() << QFile("/home/user/.cache/MeeBible/MeeBible/cache.sqlite-journal").exists();
// 
//     openDB();
// 
//     QStringList xml = Paths::allCachedXML("trans_");
//     for (int i = 0; i < xml.size(); i++)
//     {
//         qDebug() << "Deleting" << Paths::cachedXML(xml.at(i));
//         if (! QFile::remove(Paths::cachedXML(xml.at(i))))
//             qDebug() << "Can't delete" << Paths::cachedXML(xml.at(i));
//     }
// 
//     _indexer.setTranslation(0);
// 
//     QStringList idx = Paths::allIndexFiles();
//     for (int i = 0; i < idx.size(); i++)
//     {
//         qDebug() << "Deleting" << idx.at(i);
//         if (! QFile::remove(idx.at(i)))
//             qDebug() << "Can't delete" << idx.at(i);
//     }
// }




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


void Cache::search(Translation* translation, const QString& query, int maxresults)
{
    openStorage(translation);
    _indexer.setTranslation(translation);

    qDebug() << "Chapters in storage:" << _storage->entryCount();
    qDebug() << "Chapters in index  :" << _indexer.docCount();

    SearchThread* searchThread = new SearchThread(&_indexer, translation, query, maxresults, this);

    connect(searchThread, SIGNAL(finished()), this, SLOT(onSearchThreadFinished()));

    if (_storage->entryCount() != _indexer.docCount())
    {
        IndexRebuildThread *rebuildThread = new IndexRebuildThread(&_indexer, _storage, translation, this);

        connect(rebuildThread, SIGNAL(finished()), this, SLOT(onIndexRebuilt()));
        connect(rebuildThread, SIGNAL(finished()), searchThread, SLOT(start()));

        rebuildingIndex();
        rebuildThread->start();
    }
    else
        searchThread->start();
}

void Cache::onSearchThreadFinished()
{
    SearchThread* thread = dynamic_cast<SearchThread*>(sender());

    QList<SearchMatch> rawResults = thread->results();

    QList<QVariant> results;

    const Translation* translation = thread->translation();
    QStringList bookCodes = translation->bookCodes();

    QList<SearchQueryParser::QueryToken> queryTokens = SearchQueryParser::parseQuery(thread->query());

    openStorage(translation);

    foreach (const SearchMatch& match, rawResults)
    {
        QString bookCode = translation->bookCodeAt(match.bookNo);

        ChapterStorageHeader header;
        strncpy(header.bookCode, bookCode.toUtf8().data(), 5);
        header.chapterNo = match.chapterNo;
        QString html = QString::fromUtf8(_storage->load(header));
        QString text = stripHtml(html);

        QString preview = text.mid(qMax(match.offset - 15, 0), 50);

        results.append(QVariant::fromValue(SearchResult(
            Place(bookCode, match.chapterNo),
            Highlighter::highlight(preview, queryTokens, "<u>", "</u>", -1),
            match.matchesInChapter
        )));
    }

    searchFinished(results);

    thread->wait();
    thread->deleteLater();
}

void Cache::onIndexRebuilt() { indexRebuilt(); }




void Cache::openStorage(const Translation* translation)
{
    QString basename = Paths::storageBasename(translation);
    if (basename != _storageBasename)
    {
        qDebug() << "Reopening storage" << basename;
        delete _storage;
        _storage = new CacheStorage(basename);
        _storageBasename = basename;
    }
}

void Cache::openStorage(const QString& transCode, const QString& langCode)
{
    QString basename = Paths::storageBasename(transCode, langCode);
    if (basename != _storageBasename)
    {
        qDebug() << "Reopening storage" << basename;
        delete _storage;
        _storage = new CacheStorage(basename);
        _storageBasename = basename;
    }
}

void Cache::closeStorage()
{
    delete _storage;
    _storage = 0;
    _storageBasename = "";
}


void Cache::fillStorageHeader(ChapterStorageHeader *header, const Place& place)
{
    strncpy(header->bookCode, place.bookCode().toUtf8().data(), 4);
    header->bookCode[4] = '\0';
    header->chapterNo = place.chapterNo();
}


void Cache::saveChapter(const Translation* translation, const Place& place, QString html)
{
    if (html.length() == 0) return;

    QElapsedTimer timer; timer.start();

    openStorage(translation);

    ChapterStorageHeader header;
    fillStorageHeader(&header, place);
    _storage->save(header, html.toUtf8());

    qDebug() << "storage" << timer.restart();

    int bookNo = translation->bookCodes().indexOf(place.bookCode());
    QString text = stripHtml(html);
    qDebug() << "clear" << timer.restart();

    _indexer.setTranslation(translation);
    _indexer.addChapter(bookNo, place.chapterNo(), text);

    _indexer.sync();

    qDebug() << "index" << timer.restart();
}


void Cache::saveChapters(const Translation* translation, QList<QPair<Place, QString> > chapters)
{
    QElapsedTimer timer; timer.start();

    openStorage(translation);
    _indexer.setTranslation(translation);


    QList<QPair<ChapterStorageHeader, QByteArray> > toStorage;
    for (int i = 0; i < chapters.size(); i++)
    {
        const QPair<Place, QString>& chapter = chapters[i];

        toStorage.append(QPair<ChapterStorageHeader, QByteArray>(
            ChapterStorageHeader(chapter.first.bookCode().toUtf8().data(), chapter.first.chapterNo()),
            chapter.second.toUtf8()
        ));
    }
    _storage->bulkSave(toStorage);


    QStringList bookCodes = translation->bookCodes();

    for (int i = 0; i < chapters.size(); i++)
    {
        const QPair<Place, QString>& chapter = chapters[i];

        int bookNo = bookCodes.indexOf(chapter.first.bookCode());
        QString text = stripHtml(chapter.second);
        _indexer.addChapter(bookNo, chapter.first.chapterNo(), text);
    }
    _indexer.sync();

    qDebug() << "saveChapters" << timer.elapsed();
}


QString Cache::loadChapter(const Translation* translation, const Place& place)
{
    openStorage(translation);

    ChapterStorageHeader header;
    fillStorageHeader(&header, place);
    return QString::fromUtf8(_storage->load(header));
}




void Cache::clearCache() {}



QSet<QPair<QString, int> > Cache::availableChapters(const Translation* translation)
{
    openStorage(translation);

    int n = _storage->entryCount();
    ChapterStorageHeader headers[n];
    _storage->getAllHeaders(headers);

    QSet<QPair<QString, int> > result;
    for (int i = 0; i < n; i++)
        result.insert(QPair<QString, int>(
            QString::fromUtf8(headers[i].bookCode),
            headers[i].chapterNo
        ));

    return result;
}



QString Cache::stripHtml(const QString& html)
{
    QString text = html;
    text.replace(_stripStyles, " ");
    text.replace(_stripTags, " ");
    text.replace(_stripSpaces, " ");
    text.replace(QChar(0x00ad), "");

    return text;
}



bool Cache::oldStorageFound()
{
    return QFile(Paths::old_cacheDB()).exists();
}


void Cache::convertOldCacheDB()
{
    qDebug() << "Converting old storage";

    QElapsedTimer timer; timer.start();

    sqlite3* db;

    if (sqlite3_open_v2(Paths::old_cacheDB().toUtf8(), &db, SQLITE_OPEN_READONLY, 0) != SQLITE_OK)
        return;


    sqlite3_stmt* selectTrans;
    sqlite3_prepare_v2(db,
        "SELECT DISTINCT langCode, transCode FROM html", -1,
        &selectTrans, 0
    );

    sqlite3_stmt* selectChapters;
    sqlite3_prepare_v2(db,
        "SELECT bookCode, chapterNo, html FROM html WHERE langCode=? AND transCode=? ORDER BY bookNo, chapterNo", -1,
        &selectChapters, 0
    );


    sqlite3_reset(selectTrans);
    while (sqlite3_step(selectTrans) == SQLITE_ROW)
    {
        QString langCode = QString::fromUtf16((const ushort*)sqlite3_column_text16(selectTrans, 0));
        QString transCode = QString::fromUtf16((const ushort*)sqlite3_column_text16(selectTrans, 1));

        openStorage(transCode, langCode);
        _storage->clear();

        QList<QPair<ChapterStorageHeader, QByteArray> > toStorage;

        sqlite3_reset(selectChapters);
        sqlite3_bind_text16(selectChapters, 1, langCode.utf16(), -1, SQLITE_STATIC);
        sqlite3_bind_text16(selectChapters, 2, transCode.utf16(), -1, SQLITE_STATIC);

        while (sqlite3_step(selectChapters) == SQLITE_ROW)
        {
            toStorage.append(QPair<ChapterStorageHeader, QByteArray>(
                ChapterStorageHeader(
                    QString::fromUtf16((const ushort*)sqlite3_column_text16(selectChapters, 0)).toUtf8().data(),
                    sqlite3_column_int(selectChapters, 1)
                ),
                QString::fromUtf16((const ushort*)sqlite3_column_text16(selectChapters, 2)).toUtf8()
            ));
        }
        _storage->bulkSave(toStorage);
    }


    sqlite3_finalize(selectChapters);
    sqlite3_finalize(selectTrans);
    sqlite3_close(db);

    QFile(Paths::old_cacheDB()).remove();

    qDebug() << "Converting done" << timer.elapsed();
}



void Cache::closeIndex()
{
    _indexer.setTranslation(0);
}
