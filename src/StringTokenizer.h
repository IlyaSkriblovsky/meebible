#pragma once

#include <QString>

class StringTokenizer
{
    public:
        typedef void (*token_callback)(void* data, const QString& token, int start_offset, int end_offset, int number);

        static void tokenize(const QString& text, token_callback callback, void* userdata)
        {
            // QString::normalized() is incredibly slow

            const QChar* data = text.constData();
            int text_pos = 0;
            int number = 0;

            QString token;
            token.reserve(50);

            while (data[text_pos] != 0)
            {
                QChar c = data[text_pos++];
                bool in_tag = false;
                while (c != 0)
                {
                    if (c == '<') in_tag = true;
                    else if (c == '>') in_tag = false;
                    else
                    {
                        if (! in_tag  &&  ! is_spacing(c))
                            break;
                    }

                    c = data[text_pos++];
                }

                if (c == 0) return;

                int start_offset = text_pos-1;
                token.clear();

                while (c != 0 && c != '<' && ! is_spacing(c))
                {
                    c = c.toLower();
                    const QString& norm = c.decomposition();
                    if (norm.isEmpty())
                    {
                        if (! c.isMark())
                            token.append(c);
                    }
                    else
                    {
                        int norm_l = norm.length();
                        const QChar* norm_c = norm.constData();
                        for (int i = 0; i < norm_l; i++)
                            if (! norm_c[i].isMark())
                                token.append(norm_c[i]);
                    }
                    c = data[text_pos++];
                }

                text_pos--;

                callback(userdata, token, start_offset, text_pos, number++);
            }
        }

    private:
        inline static bool is_spacing(QChar c)
        {
            QChar::Category cat = c.category();
            // return
            //     (cat == QChar::Separator_Space) ||
            //     (cat == QChar::Separator_Line) ||
            //     (cat == QChar::Separator_Paragraph) ||
            //     (cat == QChar::Punctuation_Connector) ||
            //     (cat == QChar::Punctuation_Dash) ||
            //     (cat == QChar::Punctuation_Open) ||
            //     (cat == QChar::Punctuation_Close) ||
            //     (cat == QChar::Punctuation_InitialQuote) ||
            //     (cat == QChar::Punctuation_FinalQuote) ||
            //     (cat == QChar::Punctuation_Other) ||
            //     (cat == QChar::Symbol_Math) ||
            //     (cat == QChar::Symbol_Currency) ||
            //     (cat == QChar::Symbol_Modifier) ||
            //     (cat == QChar::Symbol_Other) ||
            //     (cat == QChar::Other_Control) ||
            //     (cat == QChar::Other_Format) ||
            //     (cat == QChar::Other_Surrogate) ||
            //     (cat == QChar::Other_PrivateUse) ||
            //     (cat == QChar::Other_NotAssigned);
            return (cat >= 7 && cat <= 14) || (cat >= 20 && cat <= 30);
        }
};
