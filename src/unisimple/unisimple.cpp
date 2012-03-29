#include <cstdio>
#include <cstring>
#include <cstdlib>

#include "unisimple.h"
#include "utf8.h"

struct CharClassDesc
{
    char32 codepoint;
    CharClass charclass;
};

CharClassDesc charClasses[] = {
    #include "gen_CharClasses.inc.cpp"
};


struct SimplifyDesc
{
    char32 codepoint;
    unsigned short replacement_start;
    unsigned char replacement_len;
};

char32 simplifyBlob[] = {
    #include "gen_Simplify_blob.inc.cpp"
};

SimplifyDesc simplifyDesc[] = {
    #include "gen_SimplifyDesc.inc.cpp"
};



CharClass charClass(char32 c)
{
    int l = 0;
    int r = sizeof(charClasses) / sizeof(charClasses[0]);

    while (r - l > 1)
    {
        int m = (r + l) >> 1;

        char32 mv = charClasses[m].codepoint;
        if (mv >  c) r = m;
        if (mv <= c) l = m;
    }

    return charClasses[l].charclass;
}


int simplifyChar(char32 c, char32 *out)
{
    if (charClass(c) == UCHAR_CLASS_M)
        return 0;

    int l = 0;
    int r = sizeof(simplifyDesc) / sizeof(simplifyDesc[0]);

    while (r - l > 1)
    {
        int m = (r + l ) >> 1;

        char32 mv = simplifyDesc[m].codepoint;
        if (mv >  c) r = m;
        if (mv <= c) l = m;
    }

    if (simplifyDesc[l].codepoint != c)
    {
        if (out)
            *out = c;
        return 1;
    }

    if (out)
        memcpy(out, &simplifyBlob[simplifyDesc[l].replacement_start], sizeof(char32) * simplifyDesc[l].replacement_len);
    return simplifyDesc[l].replacement_len;
}



// Simplify string encoded in UTF-8
//
// buffer argument may point to existing array with *bufsize bytes
// If *bufsize bytes will be not enough, this function will realloc() the buffer
// and store its new size in *bufsize
//
// if buffer argument is null, this function will malloc() new buffer
//
// Caller is responsible to free() buffer even if it created by function
void simplifyUTF8_alloc(const char* input, int inlen, char** buffer, int *bufsize)
{
    if (inlen == -1)
        inlen = strlen(input);

    int outlen = 0;

    if (*buffer == 0)
    {
        outlen = inlen;

        int allocsize = outlen + 1;
        if (bufsize) *bufsize = allocsize;
        *buffer = (char*)malloc(allocsize);
    }
    else
        outlen = *bufsize;

    int inpos = 0;
    int outpos = 0;
    char32 c;

    char32 simplified[20];

    while (inpos < inlen)
    {
        U8_NEXT_UNSAFE(input, inpos, c);

        int simpcount = simplifyChar(c, simplified);

        for (int i = 0; i < simpcount; i++)
        {
            if (outlen - outpos < 4)
            {
                outlen = outlen * 2;

                int allocsize = outlen + 1;
                if (bufsize) *bufsize = allocsize;
                *buffer = (char*)realloc(*buffer, allocsize);
            }

            U8_APPEND_UNSAFE(*buffer, outpos, simplified[i]);
        }
    }

    (*buffer)[outpos++] = 0;
}


// int main()
// {
//     int inpos = 0;
//     const char input[] = "Ёлки Když přišel ßÆŒĐ";
// 
//     char* output = 0;
//     int outsize = 0;
//     simplifyUTF8_alloc(input, -1, &output, &outsize);
//     printf("%s\n", output);
//     printf("strlen: %d\n", strlen(output));
//     printf("bufsize: %d\n", outsize);
//     free(output);
// }
