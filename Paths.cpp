#include "Paths.h"

#include <QDesktopServices>


QDir Paths::_cacheDir;
QDir Paths::_shareDir;
QDir Paths::_qmlDir;


void Paths::init()
{
    _cacheDir = QDir(QDesktopServices::storageLocation(QDesktopServices::CacheLocation));
    _cacheDir.mkpath("MeeBible");
    _cacheDir.cd("MeeBible");

    _shareDir = QDir("/opt/meebible/share");
    _qmlDir = QDir("/opt/meebible/qml");
}


QString Paths::cacheDB()
{
    return _cacheDir.filePath("cache.sqlite");
}


QString Paths::nwtDB()
{
    return _shareDir.filePath("nwt.sqlite");
}

QString Paths::boDB()
{
    return _shareDir.filePath("bo.sqlite");
}

QString Paths::langsDB()
{
    return _shareDir.filePath("langs.sqlite");
}


QString Paths::qmlMain()
{
    return _qmlDir.filePath("main.qml");
}
