#ifndef NWTPARSER_H
#define NWTPARSER_H

#include "Parser.h"

#include <QMap>

class NWTParser: public Parser
{
    public:
        NWTParser();

        virtual ChapterRequest* requestChapter(
            MultiSource* source,
            Translation* translation,
            QNetworkAccessManager* nam,
            const QString &bookCode,
            int chapter
        );

    protected:
        QMap<QString, QString> _urlPrefix;
};

#endif
