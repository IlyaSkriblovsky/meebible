#ifndef CACHE_H
#define CACHE_H

#include <QObject>
#include <QRegExp>

#include "Place.h"


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

    void ftstest();


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

    sqlite3_stmt* _stmt_saveChapter_fts;
    sqlite3_stmt* _stmt_ftstest;

    QRegExp _stripTags;
    QRegExp _stripSpaces;
    QRegExp _stripStyles;


    void openDB();

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
