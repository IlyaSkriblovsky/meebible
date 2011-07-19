#ifndef PATHS_H
#define PATHS_H

#include <QString>
#include <QDir>


class Paths
{
public:
    static void init();

    static QString cacheDB();


private:
    static QDir _cacheDir;
};

#endif // PATHS_H
