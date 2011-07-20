#ifndef CACHE_H
#define CACHE_H

#include <QSqlDatabase>

class Translation;


class Cache
{
public:
    static Cache* instance();

    Cache();

    void saveChapter(const Translation* translation, const QString& bookCode, int chapterNo, QString html);

    QString loadChapter(const Translation* translation, const QString& bookCode, int chapterNo);

    bool hasChapter(const Translation* translation, const QString& bookCode, int chapterNo);

private:
    static Cache* _instance;


    QSqlDatabase _db;
};

#endif // CACHE_H
