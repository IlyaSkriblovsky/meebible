#include <QApplication>
#include <QDebug>

#include "Language.h"
#include "TranslationsList.h"
#include "NWTSource.h"
#include "Translation.h"
#include "BibleView.h"


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    Language::load();

    TranslationsList translationsList;

    NWTSource nwtSource;
    nwtSource.addTranslationsToList(&translationsList);


    BibleView view;
    view.setTranslation(translationsList.translationsForLang(Language::langByCode("u"))[0]);
    view.loadChapter("mt", 5);

    view.show();

    return app.exec();
}
