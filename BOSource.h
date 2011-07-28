#ifndef BOSOURCE_H
#define BOSOURCE_H

#include "Source.h"

class TranslationsList;
class Languages;


class BOSource : public Source
{
public:
    BOSource();

    virtual void addTranslationsToList(Languages* languages, TranslationsList *list);
};

#endif // BOSOURCE_H
