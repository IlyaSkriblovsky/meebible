#include <QApplication>
#include <QDebug>

#include "Language.h"
#include "TranslationsList.h"
#include "Translation.h"
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
    if (dialog.exec() == QDialog::Accepted)
        mainWindow.setTranslation(dialog.selectedTranslation());


    mainWindow.showChapter("mt", 5);

    return app.exec();
}
