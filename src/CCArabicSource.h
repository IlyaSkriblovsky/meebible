#ifndef CCARABICSOURCE_H
#define CCARABICSOURCE_H

#include "Source.h"

class CCArabicSource: public Source
{
    public:
        CCArabicSource();

        virtual void addTranslationsToList(Languages* languages);
};

#endif
