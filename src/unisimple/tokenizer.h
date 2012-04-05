#pragma once

#include <cstdlib>

#include "unisimple.h"

class Tokenizer
{
    private:
        static inline bool isSpacingCharClass(CharClass cc)
        {
            return (cc & (UCHAR_CLASS_Z | UCHAR_CLASS_P | UCHAR_CLASS_S | UCHAR_CLASS_C)) != 0;
        }

    public:
        typedef void (*token_callback)(const char16* token, int len, int start_offset, int end_offset, int position);

        void tokenize_text(const char16* text, token_callback callback)
        {
            char16* token = 0;
            int token_pos = 0;
            int token_buf_size = 0;

            int text_pos = 0;

            int start_offset = 0;
            int position = 0;
            char16 simplified[20];


            while (text[text_pos] != 0)
            {
                char16 c = text[text_pos++];
                CharClass cc = us::charClass(c);
                bool in_tag = false;
                while (c != 0)
                {
                    if (c == '<') in_tag = true;
                    else if (c == '>') in_tag = false;
                    else
                        if (! in_tag  &&  ! isSpacingCharClass(cc))
                            break;

                    c = text[text_pos++];
                    cc = us::charClass(c);
                }

                if (c == 0) return;

                start_offset = text_pos;
                token_pos = 0;

                while (c != 0 && c != '<' && ! isSpacingCharClass(cc))
                {
                    int count = us::simplifyChar(c, simplified);
                    for (int i = 0; i < count; i++)
                    {
                        if (token_pos >= token_buf_size)
                        {
                            if (token_buf_size == 0)
                                token_buf_size = 20;
                            else
                                token_buf_size *= 2;
                            token = (char16*)realloc(token, sizeof(char16) * token_buf_size);
                        }

                        token[token_pos++] = simplified[i];
                    }

                    c = text[text_pos++];
                    cc = us::charClass(c);
                }

                text_pos--;

                callback(
                    token, token_pos,
                    start_offset, text_pos,
                    position
                );

                position++;
            }
        }
};
