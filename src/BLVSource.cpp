#include "BLVSource.h"

#include "Languages.h"
#include "BLVTranslation.h"


BLVSource::BLVSource()
{
}


void BLVSource::addTranslationsToList(Languages* languages)
{
    Language* latvian = languages->langByCode("lv");
    latvian->addTranslation(new BLVTranslation(latvian));
}
