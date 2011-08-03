#ifndef CACHE_H
#define CACHE_H

#include <QObject>
#include <QSqlDatabase>

class Translation;


class Cache: public QObject
{
    Q_OBJECT

public:
    static Cache* instance();

    Cache();
    virtual ~Cache();

    void saveChapter(const Translation* translation, const QString& bookCode, int chapterNo, QString html);

    QString loadChapter(const Translation* translation, const QString& bookCode, int chapterNo);

    bool hasChapter(const Translation* translation, const QString& bookCode, int chapterNo);

    int totalChaptersInCache(const Translation* translation);


public slots:
    void search(Translation* translation, const QString& text);


signals:
    void searchStarted();
    void matchFound(QString bookCode, int chapterNo);
    void searchFinished();


private:
    static Cache* _instance;


    QSqlDatabase _db;

private slots:
    void onThreadMatchFound(const QString& bookCode, int chapterNo);
    void onThreadFinished();
};

#endif // CACHE_H
