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
#include "Language.h"
#include "NWTSource.h"
#include "BOSource.h"
#include "TranslationsList.h"
#include "Translation.h"
#include "BibleView.h"

int main(int argc, char *argv[])
{
    qDebug() << "Hello, World!";

    QApplication app(argc, argv);

    Paths::init();
    Cache cache;

    Language::load();

    TranslationsList translationsList;

    NWTSource nwtSource;
    nwtSource.addTranslationsToList(&translationsList);
    BOSource boSource;
    boSource.addTranslationsToList(&translationsList);

    Translation* translation = translationsList.translationsForLang(Language::langByCode("u"))[0];

    QDeclarativeEngine engine;

    qmlRegisterType<Translation>();
    qmlRegisterType<BibleView>("MeeBible", 0, 1, "BibleView");

    QDeclarativeView view;
    view.rootContext()->setContextProperty("dflTranslation", (QObject*)translation);
    view.setSource(QUrl::fromLocalFile("../MeeBible2/qml/main.qml"));

    view.show();

    return app.exec();
}

#endif
