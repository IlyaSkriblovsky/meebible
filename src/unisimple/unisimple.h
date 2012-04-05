#pragma once

#include "unichar.h"

enum CharClass
{
    UCHAR_CLASS_L = 0x01,
    UCHAR_CLASS_M = 0x02,
    UCHAR_CLASS_N = 0x04,
    UCHAR_CLASS_Z = 0x08,
    UCHAR_CLASS_C = 0x10,
    UCHAR_CLASS_P = 0x20,
    UCHAR_CLASS_S = 0x40
};

namespace us
{

    CharClass charClass(char16 c);
    int simplifyChar(char16, char16 *out);

};
