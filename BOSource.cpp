#include "BOSource.h"

#include "Languages.h"
#include "BORusTranslation.h"


BOSource::BOSource()
{
}

void BOSource::addTranslationsToList(Languages* languages)
{
    Language* russian = languages->langByCode("u");
    russian->addTranslation(new BORusTranslation(russian));
}
