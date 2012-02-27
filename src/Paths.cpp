#include "Paths.h"

#include <QDebug>

#include <QDesktopServices>
#include <QCoreApplication>


#include "Settings.h"


QDir Paths::_cacheDir;
QDir Paths::_shareDir;
QDir Paths::_qmlDir;
QDir Paths::_translationsDir;


void Paths::init()
{
    _cacheDir = QDir(QDesktopServices::storageLocation(QDesktopServices::CacheLocation));
    _cacheDir.mkpath(".");

    #ifdef DEBUGPATHS
        _shareDir = QDir("share");
        _qmlDir = QDir("qml");
        _translationsDir = QDir("translations");
    #else
        #ifdef SYMBIAN
            _shareDir = QDir(QCoreApplication::applicationDirPath() + "/share");
            _qmlDir = QDir(QCoreApplication::applicationDirPath() + "/qml");
            _translationsDir = QDir(QCoreApplication::applicationDirPath() + "/translations");
        #else
            _shareDir = QDir(INSTALLPREFIX"/share");
            _qmlDir = QDir(INSTALLPREFIX"/qml");
            _translationsDir = QDir(INSTALLPREFIX"/translations");
        #endif
    #endif
}


QString Paths::cacheDB()
{
    return _cacheDir.filePath("cache.sqlite");
}


QString Paths::langsDB()
{
    return _shareDir.filePath("langs.sqlite");
}

QString Paths::transDB()
{
    return _shareDir.filePath("trans.sqlite");
}


QString Paths::qmlMain()
{
    return _qmlDir.filePath("main.qml");
}


QString Paths::script_js()
{
    return _shareDir.filePath("script.js");
}

QString Paths::style_css()
{
    return _shareDir.filePath("style.css");
}

QString Paths::nwt_xslt()
{
    return _shareDir.filePath("nwt.xslt");
}


QString Paths::translationFile(const QString& locale)
{
    return _translationsDir.filePath(QString("meebible_%1").arg(locale));
}



QUrl Paths::wsUrl(const QString& path)
{
    return QUrl(QString("http://%1/%2")
        .arg(Settings::instance()->webService())
        .arg(path))
    ;
}



QString Paths::cachedXML(const QString& name)
{
    return _cacheDir.filePath(name + ".xml");
}

QStringList Paths::allCachedXML(const QString& prefix)
{
    QStringList files = _cacheDir.entryList(QStringList("*.xml"));
    QStringList result;
    for (int i = 0; i < files.size(); i++)
    {
        QString file = files.at(i);
        if (file.startsWith(prefix))
        {
            file.replace(".xml", "");
            result.append(file);
        }
    }

    return result;
}
