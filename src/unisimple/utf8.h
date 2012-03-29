#ifndef UTF8_H
#define UTF8_H

#include "char32.h"

extern const unsigned char utf8_countTrailBytes[256];

#define U8_COUNT_TRAIL_BYTES(leadByte) (utf8_countTrailBytes[(unsigned char)leadByte])

#define U8_MASK_LEAD_BYTE(leadByte, countTrailBytes) ((leadByte)&=(1<<(6-(countTrailBytes)))-1)

#define U8_NEXT_UNSAFE(s, i, c) \
{ \
    (c)=(unsigned char)(s)[(i)++]; \
    if((unsigned char)((c)-0xc0)<0x35) { \
        unsigned char __count=U8_COUNT_TRAIL_BYTES(c); \
        U8_MASK_LEAD_BYTE(c, __count); \
        switch(__count) { \
        /* each following branch falls through to the next one */ \
        case 3: \
            (c)=((c)<<6)|((s)[(i)++]&0x3f); \
        case 2: \
            (c)=((c)<<6)|((s)[(i)++]&0x3f); \
        case 1: \
            (c)=((c)<<6)|((s)[(i)++]&0x3f); \
        /* no other branches to optimize switch() */ \
            break; \
        } \
    } \
}


#define U8_APPEND_UNSAFE(s, i, c) \
{ \
    if((char32)(c)<=0x7f) { \
        (s)[(i)++]=(unsigned char)(c); \
    } else { \
        if((char32)(c)<=0x7ff) { \
            (s)[(i)++]=(unsigned char)(((c)>>6)|0xc0); \
        } else { \
            if((char32)(c)<=0xffff) { \
                (s)[(i)++]=(unsigned char)(((c)>>12)|0xe0); \
            } else { \
                (s)[(i)++]=(unsigned char)(((c)>>18)|0xf0); \
                (s)[(i)++]=(unsigned char)((((c)>>12)&0x3f)|0x80); \
            } \
            (s)[(i)++]=(unsigned char)((((c)>>6)&0x3f)|0x80); \
        } \
        (s)[(i)++]=(unsigned char)(((c)&0x3f)|0x80); \
    } \
}

#endif
