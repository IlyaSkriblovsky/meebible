#include "BOSource.h"

#include "TranslationsList.h"
#include "Language.h"
#include "BORusTranslation.h"


BOSource::BOSource()
{
}

void BOSource::addTranslationsToList(TranslationsList *list)
{
    list->addTranslation(Language::langByCode("u"), new BORusTranslation());
}
