#include <QApplication>
#include <QDebug>
#include <QtDeclarative>
#include <QtGlobal>

#ifndef SYMBIAN
    #include <MDeclarativeCache>
#endif

#include <QTranslator>
#include <QLocale>

#include <QFontDatabase>

#include "Paths.h"
#include "Cache.h"
#include "Languages.h"
#include "Language.h"
#include "Translation.h"
#include "BibleView.h"
#include "Fetcher.h"
#include "Settings.h"
#include "MetaInfoLoader.h"
#include "Feedback.h"
#include "Bookmarks.h"
#include "Place.h"
#include "PlaceAccesser.h"
#include "StartupTracker.h"


#ifdef SYMBIAN
    #include <cstdio>
    void symbianMessageOutput(QtMsgType type, const char *msg)
    {
        FILE *f = fopen("c:\\meebible.log", "a");
        switch (type)
        {
            case QtDebugMsg:    fprintf(f, "debug> %s\n", msg); break;
            case QtWarningMsg:  fprintf(f, "warn > %s\n", msg); break;
            case QtCriticalMsg: fprintf(f, "crit > %s\n", msg); break;
            case QtFatalMsg:    fprintf(f, "fatal> %s\n", msg); abort(); break;
        }
        fclose(f);
    }
#endif


Q_DECL_EXPORT int main(int argc, char *argv[])
{
    #ifdef SYMBIAN
        qInstallMsgHandler(symbianMessageOutput);
    #endif

    #ifndef SYMBIAN
        QApplication *app = MDeclarativeCache::qApplication(argc, argv);
    #else
        QApplication *app = new QApplication(argc, argv);
    #endif
    app->setOrganizationName("MeeBible");
    app->setApplicationName("MeeBible");

    Paths::init();

    QTranslator translator;
    translator.load(Paths::translationFile(QLocale::system().name()));
    app->installTranslator(&translator);

    Cache cache;
    MetaInfoLoader metaInfoLoader;

    Languages languages;

    Settings settings(&languages);

    Feedback feedback;

    Bookmarks bookmarks;

    PlaceAccesser placeAccesser;

    QDeclarativeEngine engine;


    qmlRegisterType<Language>();
    qmlRegisterUncreatableType<Translation>("MeeBible", 0, 1, "Translation", "Translation is abstract");
    qmlRegisterType<BibleView>("MeeBible", 0, 1, "BibleView");
    qmlRegisterType<Fetcher>("MeeBible", 0, 1, "Fetcher");

    qRegisterMetaType<Place>();


    #ifndef SYMBIAN
        QDeclarativeView* view = MDeclarativeCache::qDeclarativeView();
    #else
        QDeclarativeView* view = new QDeclarativeView();
    #endif
    view->setAttribute(Qt::WA_NoSystemBackground);


    view->rootContext()->setContextProperty("languages", &languages);
    view->rootContext()->setContextProperty("cache", &cache);
    view->rootContext()->setContextProperty("metaInfoLoader", &metaInfoLoader);
    view->rootContext()->setContextProperty("settings", &settings);
    view->rootContext()->setContextProperty("feedback", &feedback);
    view->rootContext()->setContextProperty("bookmarks", &bookmarks);
    view->rootContext()->setContextProperty("placeAccesser", &placeAccesser);

    #ifdef FREEVERSION
        view->rootContext()->setContextProperty("freeversion", QVariant(true));
    #else
        view->rootContext()->setContextProperty("freeversion", QVariant(false));
    #endif


    #ifdef SYMBIAN
        qDebug() << "Symbian";
        view->rootContext()->setContextProperty("SYMBIAN", QVariant(true));
    #else
        qDebug() << "NOT Symbian";
        view->rootContext()->setContextProperty("SYMBIAN", QVariant(false));
    #endif

    #ifdef NOSEARCH
        qDebug() << "Search disabled";
        view->rootContext()->setContextProperty("NOSEARCH", QVariant(true));
    #else
        view->rootContext()->setContextProperty("NOSEARCH", QVariant(false));
    #endif

    #ifdef NOSHARE
        qDebug() << "Verse sharing disabled";
        view->rootContext()->setContextProperty("NOSHARE", QVariant(true));
    #else
        view->rootContext()->setContextProperty("NOSHARE", QVariant(false));
    #endif


    view->setSource(QUrl::fromLocalFile(Paths::qmlMain()));


    view->showFullScreen();


    StartupTracker startupTracker;
    startupTracker.sendStartupInfo();


    qDebug() << "FONTS:";
    QFontDatabase fonts;
    foreach (QString family, fonts.families())
    {
        qDebug() << "Family:" << family;
        foreach (QString style, font.styles(family))
        {
            qDebug() << "\t" << "Style:" << style;
        }
    }



    return app->exec();
}
