#ifndef CCARABICPARSER_H
#define CCARABICPARSER_H

#include "Parser.h"

#include <QMap>

class CCArabicParser: public Parser
{
    public:
        CCArabicParser();

        virtual ChapterRequest* requestChapter(
            MultiSource* source,
            Translation* translation,
            QNetworkAccessManager* nam,
            const QString &bookCode,
            int chapter
        );

    protected:
        QMap<QString, QString> _searchNames;
};


#endif
