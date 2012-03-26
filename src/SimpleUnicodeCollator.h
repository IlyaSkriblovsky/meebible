#ifndef SIMPLEUNICODECOLLATOR_H
#define SIMPLEUNICODECOLLATOR_Hy

#include "UnicodeCollator.h"

class SimpleUnicodeCollator: public UnicodeCollator
{
    public:
        virtual int find(const char16* haystack, const char16 *needle, int* firstMatch);
        virtual QString markMatches(const QString& haystack, const QString& needle, int* matchCount);
};

#endif
