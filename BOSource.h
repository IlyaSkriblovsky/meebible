#ifndef BOSOURCE_H
#define BOSOURCE_H

#include "Source.h"

class Languages;


class BOSource : public Source
{
public:
    BOSource();

    virtual void addTranslationsToList(Languages* languages);
};

#endif // BOSOURCE_H
