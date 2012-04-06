#include "Paths.h"

#include <QDebug>

#include <QDesktopServices>
#include <QCoreApplication>


#include "Settings.h"
#include "Translation.h"
#include "Language.h"


QDir Paths::_cacheDir;
QDir Paths::_shareDir;
QDir Paths::_qmlDir;
QDir Paths::_translationsDir;


QString Paths::correctSlashes(const QString& path)
{
    #ifdef SYMBIAN
        QString tmp = path;
        return tmp.replace("/", "\\");
    #else
        return path;
    #endif
}


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
    return correctSlashes(_cacheDir.filePath("cache.sqlite"));
}


QString Paths::langsDB()
{
    return correctSlashes(_shareDir.filePath("langs.sqlite"));
}

QString Paths::transDB()
{
    return correctSlashes(_shareDir.filePath("trans.sqlite"));
}


QString Paths::qmlMain()
{
    return correctSlashes(_qmlDir.filePath("main.qml"));
}


QString Paths::script_js()
{
    return correctSlashes(_shareDir.filePath("script.js"));
}

QString Paths::style_css()
{
    return correctSlashes(_shareDir.filePath("style.css"));
}

QString Paths::nwt_xslt()
{
    return correctSlashes(_shareDir.filePath("nwt.xslt"));
}


QString Paths::translationFile(const QString& locale)
{
    return correctSlashes(_translationsDir.filePath(QString("meebible_%1").arg(locale)));
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
    return correctSlashes(_cacheDir.filePath(name + ".xml"));
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
            result.append(correctSlashes(file));
        }
    }

    return result;
}



QString Paths::indexFile(const Translation* translation)
{
    return correctSlashes(_cacheDir.filePath(translation->code() + "_" + translation->language()->code() + ".idx"));
}
