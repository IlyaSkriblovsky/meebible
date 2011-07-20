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

    MainWindow mainWindow;
    mainWindow.show();

    TranslationDialog dialog(
        &translationsList,
        translationsList.translationsForLang(Language::langByCode("u"))[1]
    );
    if (dialog.exec() == QDialog::Accepted)
    {
        mainWindow.setTranslation(dialog.selectedTranslation());

        qDebug() << dialog.selectedTranslation()->bookName("1ch");

        BORusTranslation *bo = dynamic_cast<BORusTranslation*>(dialog.selectedTranslation());
        if (bo)
            qDebug() << bo->chapterUrl("de", 5);
    }


    mainWindow.showChapter("mt", 5);

    return app.exec();
}
