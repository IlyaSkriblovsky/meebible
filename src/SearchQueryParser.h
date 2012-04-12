#pragma once

#include <QString>
#include <QList>

class SearchQueryParser
{
    public:
        struct QueryToken
        {
            QString token;
            bool prefix;

            QueryToken(const QString& token, bool prefix)
                : token(token), prefix(prefix)
            { }
        };


        static QList<QueryToken> parseQuery(const QString& query);

    private:
        struct Closure
        {
            const QString* query;
            QList<QueryToken> tokens;
        };

        static void onToken(void* data, const QString& token, int startPos, int endPos, int number);
};
