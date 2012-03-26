#ifndef UNICODECOLLATOR_H
#define UNICODECOLLATOR_H

#include <QString>

#define char16  unsigned short

class UnicodeCollator
{
    public:
        virtual int find(const char16* haystack, const char16 *needle, int* firstMatch) = 0;
        virtual QString markMatches(const QString& haystack, const QString& needle, int* matchCount) = 0;
};

#endif
