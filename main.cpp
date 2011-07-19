#include <QApplication>
#include <QDebug>

#include "Language.h"
#include "TranslationsList.h"
#include "NWTSource.h"
#include "Cache.h"
#include "Paths.h"
#include "MainWindow.h"
#include "Place.h"
#include "TranslationDialog.h"


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    Paths::init();
    Cache cache;

    Language::load();

    TranslationsList translationsList;

    NWTSource nwtSource;
    nwtSource.addTranslationsToList(&translationsList);

    MainWindow mainWindow;
    mainWindow.show();

    TranslationDialog dialog(&translationsList);
    dialog.exec();

    return app.exec();
}
