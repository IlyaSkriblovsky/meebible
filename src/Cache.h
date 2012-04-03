#ifndef CACHE_H
#define CACHE_H

#include <QObject>
#include <QRegExp>
#include <QSet>
#include <QPair>

#include "Place.h"


class QThread;


class sqlite3;
class sqlite3_stmt;

class Translation;


class Cache: public QObject
{
    Q_OBJECT

    #ifndef NOSEARCH
        Q_PROPERTY(bool searchInProgress READ searchInProgress NOTIFY searchInProgressChanged)
    #endif

public:
    static Cache* instance();

    Cache();
    virtual ~Cache();

    void saveChapter(const Translation* translation, const Place& place, QString html);

    QString loadChapter(const Translation* translation, const Place& place);

    bool hasChapter(const Translation* translation, const Place& place);

    int totalChaptersInCache(const Translation* translation);


    QSet<QPair<QString, int> > availableChapters(const Translation* translation);


    void beginTransaction();
    void commitTransaction();


    #ifndef NOSEARCH
        bool searchInProgress() const { return _searchInProgress; }
    #endif


    void saveXML(const QString& name, const QString& content);
    bool hasXML(const QString& name);
    QString loadXML(const QString& name);


public slots:
    #ifndef NOSEARCH
        void search(Translation* translation, const QString& text);
    #endif

    void clearCache();


signals:
    #ifndef NOSEARCH
        void searchStarted();
        void matchFound(Place place, QString match, int matchCount);
        void searchFinished();
        void searchInProgressChanged();
    #endif


private:
    static Cache* _instance;

    sqlite3* _db;

    sqlite3_stmt* _stmt_saveChapter;
    sqlite3_stmt* _stmt_loadChapter;
    sqlite3_stmt* _stmt_hasChapter;
    sqlite3_stmt* _stmt_totalChapters;
    sqlite3_stmt* _stmt_availableChapters;

    sqlite3_stmt* _stmt_ftstest;

    QRegExp _stripTags;
    QRegExp _stripSpaces;
    QRegExp _stripStyles;


    #ifdef ASYNC_DB_IO
        QThread* _asyncThread;
    #endif

    void execWithCheck(const char* sql);

    void openDB();
    void closeDB();

    #ifndef NOSEARCH
        bool _searchInProgress;
    #endif

private slots:
    #ifndef NOSEARCH
        void onThreadMatchFound(Place place, QString match, int matchCount);
        void onThreadFinished();
    #endif
};

#endif // CACHE_H
