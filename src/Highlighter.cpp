#include "Highlighter.h"

#include "StringTokenizer.h"



QString Highlighter::highlight(
    const QString& original,
    const QList<SearchQueryParser::QueryToken>& tokens,
    const QString& before, const QString& after,
    int cutoffDistance,
    int* count/* = 0*/
)
{
    Highlighter closure;
    // closure._original = original;
    closure._tokens = tokens;
    // closure._before = before;
    // closure._after = after;
    // closure._lastPos = 0;
    // closure._count = 0;


    StringTokenizer::tokenize(original, onToken, &closure);

    // closure._result.append(closure._original.mid(closure._lastPos));

    // if (count) *count = closure._count;

    // return closure._result.join("");


    bool hasToken[tokens.size()];
    memset(hasToken, 0, sizeof(hasToken));

    int prevTokenNo = -1;
    int groupStart = 0;

    QList<Match> goodMatches;

    if (cutoffDistance == -1)
        goodMatches = closure._matches;
    else
    {
        for (int i = 0; i <= closure._matches.size(); i++)
        {
            if (i == closure._matches.size() || (prevTokenNo != -1 && closure._matches[i].number - prevTokenNo > cutoffDistance))
            {
                bool all = true;
                for (int j = 0; j < tokens.size(); j++)
                    if (! hasToken[j])
                    { all = false; break; }

                if (all)
                {
                    for (int j = groupStart; j < i; j++)
                        goodMatches.append(closure._matches[j]);
                }

                memset(hasToken, 0, sizeof(hasToken));
                groupStart = i;
            }

            if (i < closure._matches.size())
            {
                hasToken[closure._matches[i].token_i] = true;
                prevTokenNo = closure._matches[i].number;
            }
        }
    }

    int highlighted = 0;

    QStringList result;
    int lastPos = 0;
    foreach (const Match& match, goodMatches)
    {
        result.append(original.mid(lastPos, match.startPos - lastPos));
        result.append(before);
        result.append(original.mid(match.startPos, match.endPos - match.startPos));
        result.append(after);

        lastPos = match.endPos;

        highlighted++;
    }
    result.append(original.mid(lastPos));

    if (count) *count = highlighted;

    return result.join("");
}



void Highlighter::onToken(void* data, const QString& token, int startPos, int endPos, int number)
{
    Highlighter* self = static_cast<Highlighter*>(data);

    for (int i = 0; i < self->_tokens.size(); i++)
    {
        bool match = false;
        if (self->_tokens[i].prefix)
            match = token.startsWith(self->_tokens[i].token);
        else
            match = token == self->_tokens[i].token;

        if (match)
        {
            Match match;
            match.token_i = i;
            match.number = number;
            match.startPos = startPos;
            match.endPos = endPos;
            self->_matches.append(match);


            // self->_result.append(self->_original.mid(self->_lastPos, startPos - self->_lastPos));
            // self->_result.append(self->_before);
            // self->_result.append(self->_original.mid(startPos, endPos - startPos));
            // self->_result.append(self->_after);

            // self->_lastPos = endPos;
            // self->_count++;
        }
    }
}
