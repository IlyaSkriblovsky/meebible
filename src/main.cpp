#include <QApplication>
#include <QDebug>
#include <QtDeclarative>
#include <QtGlobal>

#include <MDeclarativeCache>
#include <QElapsedTimer>

#include <QTranslator>
#include <QLocale>

#include "Paths.h"
#include "Cache.h"
#include "Languages.h"
#include "Language.h"
#include "NWTSource.h"
#include "BOSource.h"
#include "BLVSource.h"
#include "CCArabicSource.h"
#include "Translation.h"
#include "BibleView.h"
#include "Fetcher.h"
#include "Settings.h"

Q_DECL_EXPORT int main(int argc, char *argv[])
{
    QElapsedTimer timer;
    timer.start();

    QApplication *app = MDeclarativeCache::qApplication(argc, argv);
    app->setOrganizationName("MeeBible");
    app->setApplicationName("MeeBible");

    Paths::init();

    QTranslator translator;
//    translator.load(Paths::translationFile("ru"));
    translator.load(Paths::translationFile(QLocale::system().name()));
    app->installTranslator(&translator);

    Cache cache;

    Languages languages;


    NWTSource nwtSource;
    nwtSource.addTranslationsToList(&languages);
    BOSource boSource;
    boSource.addTranslationsToList(&languages);
    BLVSource blvSource;
    blvSource.addTranslationsToList(&languages);
    CCArabicSource ccarabicSource;
    ccarabicSource.addTranslationsToList(&languages);

    Settings settings(&languages);

    QDeclarativeEngine engine;


    qmlRegisterType<Language>();
    qmlRegisterUncreatableType<Translation>("MeeBible", 0, 1, "Translation", "Translation is abstract");
    qmlRegisterType<BibleView>("MeeBible", 0, 1, "BibleView");
    qmlRegisterType<Fetcher>("MeeBible", 0, 1, "Fetcher");


    QDeclarativeView* view = MDeclarativeCache::qDeclarativeView();
    view->setAttribute(Qt::WA_NoSystemBackground);


    view->rootContext()->setContextProperty("languages", &languages);
    view->rootContext()->setContextProperty("cache", &cache);
    view->rootContext()->setContextProperty("settings", &settings);

    #ifdef FREEVERSION
        view->rootContext()->setContextProperty("freeversion", true);
    #else
        view->rootContext()->setContextProperty("freeversion", false);
    #endif


    view->setSource(QUrl::fromLocalFile(Paths::qmlMain()));


    view->showFullScreen();



    return app->exec();
}
