#include "SimpleSource.h"

#include "Languages.h"
#include "SimpleTranslation.h"

SimpleSource::SimpleSource(const QString& langCode)
    :_langCode(langCode)
{
}

void SimpleSource::addTranslationsToList(Languages* languages)
{
    Language* lang = languages->langByCode(_langCode);
    lang->addTranslation(createTranslation(lang));
}


Translation* SimpleSource::createTranslation(const Language* language)
{
    return new SimpleTranslation(this, language);
}
