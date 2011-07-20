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
