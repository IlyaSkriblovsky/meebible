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
    verses << 2 << 3 << 4 << 8 << 9 << 10 << 15 << 95;
    Place place("jude", 1, verses);
    const Translation* translation = translationsList.translationsForLang(Language::langByCode("u"))[0];
    qDebug() << place.toString(translation);
    qDebug() << place.prevChapter(translation).toString(translation);
    qDebug() << place.nextChapter(translation).toString(translation);
    qDebug() << place.isValid(translation);


    MainWindow mainWindow;
    mainWindow.show();

    return app.exec();
}
