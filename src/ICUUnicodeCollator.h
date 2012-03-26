#ifndef ICUUNICODECOLLATOR_H
#define ICUUNICODECOLLATOR_H

#include "UnicodeCollator.h"

class UCollator;

class ICUUnicodeCollator: public UnicodeCollator
{
    public:
        ICUUnicodeCollator();
        ~ICUUnicodeCollator();

        virtual int find(const char16* haystack, const char16 *needle, int* firstMatch);
        virtual QString markMatches(const QString& haystack, const QString& needle, int* matchCount);


    private:
        UCollator* _icuCollator;
};

#endif
