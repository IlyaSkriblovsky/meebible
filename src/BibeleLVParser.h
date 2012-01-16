#ifndef BIBELELVPARSER_H
#define BIBELELVPARSER_H

#include "Parser.h"

class BibeleLVParser: public Parser
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
