#pragma once

#include <cstring>


#include "AppendStorage.h"


struct ChapterStorageHeader
{
    char bookCode[5];
    unsigned char chapterNo;

    unsigned char _reserved[6];

    ChapterStorageHeader()
    {
        memset(bookCode, 0, 5);
        chapterNo = 0;
        memset(_reserved, 0, sizeof(_reserved));
    }

    ChapterStorageHeader(const char* bookCode, unsigned int chapterNo)
    {
        strncpy(this->bookCode, bookCode, 5);
        this->chapterNo = chapterNo;
        memset(_reserved, 0, sizeof(_reserved));
    }

    bool operator == (const ChapterStorageHeader& other) const
    {
        return strncmp(bookCode, other.bookCode, 4) == 0 && chapterNo == other.chapterNo;
    }

    QString toString() const
    {
        return QString("<%1 %2>").arg(bookCode).arg(chapterNo);
    }
} __attribute__((packed));


typedef appendstorage::AppendStorage<ChapterStorageHeader> CacheStorage;
