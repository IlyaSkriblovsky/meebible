#pragma once

#include "FileTree.h"

struct MatchEntry
{
    uint32 docid;
    uint16 token_no;

    MatchEntry(uint32 docid, uint16 token_no)
        :docid(docid), token_no(token_no)
    {
    }
} __attribute__((packed));

typedef FileTree<0x7869, 1, MatchEntry, 4> IndexTree;
