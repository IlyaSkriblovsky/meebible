#ifndef CACHE_H
#define CACHE_H

#include <QSqlDatabase>
#include <QByteArray>

class Translation;


class Cache
{
public:
    static Cache* instance();

    Cache();

    void saveChapter(const Translation* translation, const QString& bookCode, int chapterNo, QByteArray html);

    QByteArray loadChapter(const Translation* translatoin, const QString& bookCode, int chapterNo);

private:
    static Cache* _instance;


    QSqlDatabase _db;
};

#endif // CACHE_H
