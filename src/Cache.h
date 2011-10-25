#ifndef CACHE_H
#define CACHE_H

#include <QObject>
#include <QRegExp>
#include <QSqlDatabase>

class Translation;


class Cache: public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool searchInProgress READ searchInProgress NOTIFY searchInProgressChanged)

public:
    static Cache* instance();

    Cache();
    virtual ~Cache();

    void saveChapter(const Translation* translation, const QString& bookCode, int chapterNo, QString html);

    QString loadChapter(const Translation* translation, const QString& bookCode, int chapterNo);

    bool hasChapter(const Translation* translation, const QString& bookCode, int chapterNo);

    int totalChaptersInCache(const Translation* translation);


    bool searchInProgress() const { return _searchInProgress; }


public slots:
    void search(Translation* translation, const QString& text);


signals:
    void searchStarted();
    void matchFound(QString bookCode, int chapterNo, QString match, int matchCount);
    void searchFinished();
    void searchInProgressChanged();


private:
    static Cache* _instance;

    QSqlDatabase _db;

    QRegExp _stripTags;
    QRegExp _stripSpaces;
    QRegExp _stripStyles;

    bool _searchInProgress;

private slots:
    void onThreadMatchFound(const QString& bookCode, int chapterNo, QString match, int matchCount);
    void onThreadFinished();
};

#endif // CACHE_H
