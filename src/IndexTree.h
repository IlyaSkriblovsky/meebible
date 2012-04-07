#pragma once

#include "FileTree.h"

struct MatchEntry
{
    uint8 bookNo;
    uint8 chapterNo;
    uint16 tokenNo;
    uint16 offset;

    MatchEntry(uint8 bookNo, uint8 chapterNo, uint16 tokenNo, uint16 offset)
        : bookNo(bookNo), chapterNo(chapterNo), tokenNo(tokenNo), offset(offset)
    {
    }
} __attribute__((packed));


inline bool operator < (const MatchEntry& one, const MatchEntry& two)
{
    if (one.bookNo < two.bookNo) return true;
    else if (one.bookNo > two.bookNo) return false;

    if (one.chapterNo < two.chapterNo) return true;
    else if (one.chapterNo > two.chapterNo) return false;

    if (one.tokenNo < two.tokenNo) return true;
    else if (one.tokenNo > two.tokenNo) return false;

    return false;
}


inline bool operator == (const MatchEntry& one, const MatchEntry& two)
{
    return one.bookNo == two.bookNo && one.chapterNo == two.chapterNo && one.tokenNo == two.tokenNo && one.offset == two.offset;
}

inline uint qHash(const MatchEntry& key)
{
    return qHash(key.bookNo) ^ qHash(key.chapterNo) ^ qHash(key.tokenNo) ^ qHash(key.offset);
}


typedef FileTree<0x7869, 1, MatchEntry, 4> IndexTree;
