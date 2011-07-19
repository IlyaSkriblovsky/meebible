#ifndef CACHE_H
#define CACHE_H

#include <QSqlDatabase>


class Cache
{
public:
    Cache();


private:
    QSqlDatabase _db;
};

#endif // CACHE_H
