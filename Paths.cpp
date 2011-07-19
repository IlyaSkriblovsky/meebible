#include "Paths.h"

#include <QDesktopServices>


QDir Paths::_cacheDir;


void Paths::init()
{
    _cacheDir = QDir(QDesktopServices::storageLocation(QDesktopServices::CacheLocation));
    _cacheDir.mkpath("MeeBible");
    _cacheDir.cd("MeeBible");
}


QString Paths::cacheDB()
{
    return _cacheDir.filePath("cache.sqlite");
}
