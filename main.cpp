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

#include "Paths.h"
#include "Cache.h"
#include "Languages.h"
#include "Language.h"
#include "NWTSource.h"
#include "BOSource.h"
#include "Translation.h"
#include "BibleView.h"
#include "Fetcher.h"

int main(int argc, char *argv[])
{
    qDebug() << "Hello, World!";

    QApplication app(argc, argv);

    Paths::init();

    Cache cache;

    Languages languages;


    NWTSource nwtSource;
    nwtSource.addTranslationsToList(&languages);
    BOSource boSource;
    boSource.addTranslationsToList(&languages);

    QDeclarativeEngine engine;

    qmlRegisterType<Language>();
    qmlRegisterUncreatableType<Translation>("MeeBible", 0, 1, "Translation", "Translation is abstract");
    qmlRegisterType<BibleView>("MeeBible", 0, 1, "BibleView");
    qmlRegisterType<Fetcher>("MeeBible", 0, 1, "Fetcher");

    QDeclarativeView view;

    qDebug() << languages.rowCount();

    view.rootContext()->setContextProperty("languages", &languages);
    view.rootContext()->setContextProperty("cache", &cache);
    view.setSource(QUrl::fromLocalFile("../MeeBible2/qml/main.qml"));

    view.showFullScreen();


    return app.exec();
}

#endif
