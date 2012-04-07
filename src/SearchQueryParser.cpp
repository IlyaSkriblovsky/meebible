#include "SearchQueryParser.h"

#include "StringTokenizer.h"


QList<SearchQueryParser::QueryToken> SearchQueryParser::parseQuery(const QString& query)
{
    Closure closure;
    closure.query = &query;

    StringTokenizer::tokenize(query, onToken, &closure);

    return closure.tokens;
}


void SearchQueryParser::onToken(void* data, const QString& token, int startPos, int endPos, int number)
{
    Closure* closure = static_cast<Closure*>(data);

    closure->tokens.append(QueryToken(
        token,
        closure->query->length() > endPos && closure->query->at(endPos) == '*'
    ));
}
