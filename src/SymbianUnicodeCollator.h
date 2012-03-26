#ifndef SYMBIANUNICODECOLLATOR_H
#define SYMBIANUNICODECOLLATOR_H

#include "UnicodeCollator.h"

class SymbianUnicodeCollator: public UnicodeCollator
{
    public:
        virtual int find(const char16* haystack, const char16 *needle, int* firstMatch);
        virtual QString markMatches(const QString& haystack, const QString& needle, int* matchCount);
};

#endif
