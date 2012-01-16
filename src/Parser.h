#ifndef PARSER_H
#define PARSER_H

#include <QString>

class QNetworkAccessManager;

class MultiSource;
class Translation;
class ChapterRequest;

class Parser
{
    public:
        virtual ~Parser();

        virtual ChapterRequest* requestChapter(
            MultiSource* source,
            Translation* translation,
            QNetworkAccessManager *nam,
            const QString &bookCode,
            int chapter
        ) = 0;
};

#endif
