#include "Cache.h"

#include <QDebug>
#include <QDesktopServices>


Cache::Cache()
{

    qDebug() <<
    QDesktopServices::storageLocation(QDesktopServices::CacheLocation);
//    _db = QSqlDatabase::addDatabase("QSQLITE", "cache");
//    _db.setDatabaseName(
//        QDesktopServices::storageLocation(QDesktopServices::CacheLocation)
//    );
//
//    _db.execute("CREATE TABLE IF NOT EXISTS html (langCode VARCHAR, bookCode VARCHAR, chapterNo INTEGER, html, PRIMARY KEY (langCode, bookCode, chapterNo))");
}
