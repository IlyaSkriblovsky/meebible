#include "CCArabicSource.h"

#include "Languages.h"
#include "CCArabicTranslation.h"

CCArabicSource::CCArabicSource()
{
}

void CCArabicSource::addTranslationsToList(Languages* languages)
{
    Language* arabic = languages->langByCode("a");
    arabic->addTranslation(new CCArabicTranslation(arabic));
}
