#ifndef BIBLEONLINEPARSER_H
#define BIBLEONLINEPARSER_H

#include "Parser.h"

class BibleOnlineParser: public Parser
{
    public:
        virtual ChapterRequest* requestChapter(
            MultiSource* source,
            Translation* translation,
            QNetworkAccessManager* nam,
            const QString &bookCode,
            int chapter
        );

    protected:
        QString chapterUrl(Translation* translation, const QString &bookCode, int chapterNo) const;
};

#endif
