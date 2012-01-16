#ifndef KJBOPARSER_H
#define KJBOPARSER_H

#include "Parser.h"

#include <QMap>

class KJBOParser: public Parser
{
    public:
        virtual ChapterRequest* requestChapter(
            MultiSource* source,
            Translation* translation,
            QNetworkAccessManager* nam,
            const QString &bookCode,
            int chapter
        );
};


#endif
