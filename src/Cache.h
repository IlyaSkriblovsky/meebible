#ifndef CACHE_H
#define CACHE_H

#include <QObject>
#include <QRegExp>
#include <QSqlDatabase>

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

    void saveChapter(const Translation* translation, const QString& bookCode, int chapterNo, QString html);

    QString loadChapter(const Translation* translation, const QString& bookCode, int chapterNo);

    bool hasChapter(const Translation* translation, const QString& bookCode, int chapterNo);

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


signals:
    #ifndef NOSEARCH
        void searchStarted();
        void matchFound(QString bookCode, int chapterNo, QString match, int matchCount);
        void searchFinished();
        void searchInProgressChanged();
    #endif


private:
    static Cache* _instance;

    QSqlDatabase _db;

    QRegExp _stripTags;
    QRegExp _stripSpaces;
    QRegExp _stripStyles;


    void openDB();

    #ifndef NOSEARCH
        bool _searchInProgress;
    #endif

private slots:
    #ifndef NOSEARCH
        void onThreadMatchFound(const QString& bookCode, int chapterNo, QString match, int matchCount);
        void onThreadFinished();
    #endif
};

#endif // CACHE_H
