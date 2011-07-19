#include <QApplication>
#include <QDebug>

#include "Language.h"
#include "TranslationsList.h"
#include "NWTSource.h"
#include "Cache.h"
#include "Paths.h"
#include "MainWindow.h"
#include "Place.h"


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    Paths::init();
    Cache cache;

    Language::load();

    TranslationsList translationsList;

    NWTSource nwtSource;
    nwtSource.addTranslationsToList(&translationsList);


    QSet<int> verses;
    qDebug() << Place("mt", 5, verses).toString(translationsList.translationsForLang(Language::langByCode("u"))[0]);


    MainWindow mainWindow;
    mainWindow.show();

    return app.exec();
}
