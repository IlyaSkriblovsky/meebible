#ifndef BOSOURCE_H
#define BOSOURCE_H

#include "Source.h"

class TranslationsList;


class BOSource : public Source
{
public:
    BOSource();

    virtual void addTranslationsToList(TranslationsList *list);
};

#endif // BOSOURCE_H
