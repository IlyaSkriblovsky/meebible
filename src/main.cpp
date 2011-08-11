#if 0

#include <QApplication>
#include <QDebug>

#include "Language.h"
#include "TranslationsList.h"
#include "Translation.h"
#include "NWTSource.h"
#include "BOSource.h"
#include "Cache.h"
#include "Paths.h"
#include "MainWindow.h"
#include "Place.h"
#include "TranslationDialog.h"

#include "BORusTranslation.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    Paths::init();
    Cache cache;

    Language::load();

    TranslationsList translationsList;

    NWTSource nwtSource;
    nwtSource.addTranslationsToList(&translationsList);

    BOSource boSource;
    boSource.addTranslationsToList(&translationsList);

    MainWindow mainWindow(&translationsList);
    mainWindow.setTranslation(translationsList.translationsForLang(Language::langByCode("u"))[1]);
    mainWindow.show();

    mainWindow.showChapter("mt", 27);

    return app.exec();
}

#else

#include <QApplication>
#include <QDebug>
#include <QtDeclarative>
#include <QtGlobal>

#include <MDeclarativeCache>
#include <QElapsedTimer>

#include "Paths.h"
#include "Cache.h"
#include "Languages.h"
#include "Language.h"
#include "NWTSource.h"
#include "BOSource.h"
#include "Translation.h"
#include "BibleView.h"
#include "Fetcher.h"
#include "Settings.h"

Q_DECL_EXPORT int main(int argc, char *argv[])
{
    QElapsedTimer timer;
    timer.start();
    qDebug() << "Hello, World!";

    QApplication *app = MDeclarativeCache::qApplication(argc, argv);
    app->setOrganizationName("MeeBible");
    app->setApplicationName("MeeBible");

    qDebug() << "1" << timer.elapsed();

    Paths::init();

    qDebug() << "2" << timer.elapsed();

    Cache cache;

    qDebug() << "3" << timer.elapsed();

    Languages languages;

    qDebug() << "4" << timer.elapsed();


    NWTSource nwtSource;
    qDebug() << "5" << timer.elapsed();
    nwtSource.addTranslationsToList(&languages);
    qDebug() << "6" << timer.elapsed();
    BOSource boSource;
    qDebug() << "7" << timer.elapsed();
    boSource.addTranslationsToList(&languages);
    qDebug() << "8" << timer.elapsed();

    Settings settings(&languages);

    QDeclarativeEngine engine;

    qDebug() << "9" << timer.elapsed();

    qmlRegisterType<Language>();
    qmlRegisterUncreatableType<Translation>("MeeBible", 0, 1, "Translation", "Translation is abstract");
    qmlRegisterType<BibleView>("MeeBible", 0, 1, "BibleView");
    qmlRegisterType<Fetcher>("MeeBible", 0, 1, "Fetcher");

    qDebug() << "10" << timer.elapsed();

    QDeclarativeView* view = MDeclarativeCache::qDeclarativeView();
    view->setAttribute(Qt::WA_NoSystemBackground);

    qDebug() << "11" << timer.elapsed();

    view->rootContext()->setContextProperty("languages", &languages);
    view->rootContext()->setContextProperty("cache", &cache);
    view->rootContext()->setContextProperty("settings", &settings);

    qDebug() << "12" << timer.elapsed();

    view->setSource(QUrl::fromLocalFile(Paths::qmlMain()));

    qDebug() << "13" << timer.elapsed();

    view->showFullScreen();

    qDebug() << "14" << timer.elapsed();


    return app->exec();
}

#endif
