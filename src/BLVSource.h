#ifndef BLVSOURCE_H
#define BLVSOURCE_H

#include "Source.h"

class BLVSource: public Source
{
    public:
        BLVSource();

        virtual void addTranslationsToList(Languages* languages);
};

#endif
