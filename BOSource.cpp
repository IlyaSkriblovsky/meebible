#include "BOSource.h"

#include "TranslationsList.h"
#include "Languages.h"
#include "BORusTranslation.h"


BOSource::BOSource()
{
}

void BOSource::addTranslationsToList(Languages* languages, TranslationsList *list)
{
    const Language* russian = languages->langByCode("u");
    list->addTranslation(russian, new BORusTranslation(russian));
}
