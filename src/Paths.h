#ifndef PATHS_H
#define PATHS_H

#include <QString>
#include <QStringList>
#include <QDir>
#include <QUrl>


class Paths
{
public:
    static void init();

    static QString cacheDB();

    static QString langsDB();

    static QString transDB();

    static QString qmlMain();

    static QString script_js();
    static QString style_css();

    static QString nwt_xslt();

    static QString translationFile(const QString& locale);

    static QUrl wsUrl(const QString& path);

    static QString cachedXML(const QString& name);
    static QStringList allCachedXML();


private:
    static QDir _cacheDir;
    static QDir _shareDir;
    static QDir _qmlDir;
    static QDir _translationsDir;
};

#endif // PATHS_H
