#include <QApplication>
#include <QDebug>
#include <QtDeclarative>
#include <QtGlobal>

#include <MDeclarativeCache>

#include <QTranslator>
#include <QLocale>

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

#include "StartupTracker.h"


Q_DECL_EXPORT int main(int argc, char *argv[])
{
    QApplication *app = MDeclarativeCache::qApplication(argc, argv);
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

    QDeclarativeEngine engine;


    qmlRegisterType<Language>();
    qmlRegisterUncreatableType<Translation>("MeeBible", 0, 1, "Translation", "Translation is abstract");
    qmlRegisterType<BibleView>("MeeBible", 0, 1, "BibleView");
    qmlRegisterType<Fetcher>("MeeBible", 0, 1, "Fetcher");

    qRegisterMetaType<Place>();


    QDeclarativeView* view = MDeclarativeCache::qDeclarativeView();
    view->setAttribute(Qt::WA_NoSystemBackground);


    view->rootContext()->setContextProperty("languages", &languages);
    view->rootContext()->setContextProperty("cache", &cache);
    view->rootContext()->setContextProperty("metaInfoLoader", &metaInfoLoader);
    view->rootContext()->setContextProperty("settings", &settings);
    view->rootContext()->setContextProperty("feedback", &feedback);
    view->rootContext()->setContextProperty("bookmarks", &bookmarks);

    #ifdef FREEVERSION
        view->rootContext()->setContextProperty("freeversion", true);
    #else
        view->rootContext()->setContextProperty("freeversion", false);
    #endif


    #ifdef NOSEARCH
        qDebug() << "Search disabled";
        view->rootContext()->setContextProperty("NOSEARCH", true);
    #else
        view->rootContext()->setContextProperty("NOSEARCH", false);
    #endif


    view->setSource(QUrl::fromLocalFile(Paths::qmlMain()));


    view->showFullScreen();


    StartupTracker startupTracker;
    startupTracker.sendStartupInfo();



    return app->exec();
}
