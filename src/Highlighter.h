#pragma once

#include <QString>
#include <QStringList>

#include "SearchQueryParser.h"

class Highlighter
{
    public:
        static QString highlight(
            const QString& original,
            const QList<SearchQueryParser::QueryToken>& tokens,
            const QString& before, const QString& after,
            int cutoffDistance,
            int* count = 0
        );

    private:
        Highlighter() { }

        QString _original;
        QList<SearchQueryParser::QueryToken> _tokens;
        QString _before;
        QString _after;

        int _lastPos;
        int _count;

        QStringList _result;


        struct Match
        {
            int token_i;

            int number;
            int startPos;
            int endPos;
        };
        QList<Match> _matches;


        static void onToken(void* data, const QString& token, int startPos, int endPos, int number);
};
