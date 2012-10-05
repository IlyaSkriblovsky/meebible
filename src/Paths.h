#ifndef PATHS_H
#define PATHS_H

#include <QString>
#include <QStringList>
#include <QDir>
#include <QUrl>

class Translation;


class Paths
{
public:
    static void init();

    static QString old_cacheDB();

    static QString langsDB();

    static QString transDB();

    static QString qmlMain();
    static QString qmlConvertDialog();

    static QString script_js();
    static QString style_css();

    static QString nwt_xslt();

    static QString translationFile(const QString& locale);

    static QUrl wsUrl(const QString& path);

    static QString cachedXML(const QString& name);
    static QStringList allCachedXML(const QString& prefix);

    static QString indexFile(const Translation* translation);
    static QStringList allIndexFiles();

    static QString storageBasename(const Translation* translation);
    static QString storageBasename(const QString& transCode, const QString& langCode);


    static QDir cacheDir() { return _cacheDir; }


private:
    static QDir _cacheDir;
    static QDir _shareDir;
    static QDir _qmlDir;
    static QDir _translationsDir;

    static QString correctSlashes(const QString& path);
};

#endif // PATHS_H
