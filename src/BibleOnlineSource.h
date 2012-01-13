#ifndef BIBLEONLINESOURCE_H
#define BIBLEONLINESOURCE_H

#include "MultiSource.h"


class BibleOnlineSource: public MultiSource
{
    public:
        BibleOnlineSource();

        virtual ChapterRequest* requestChapter(Translation* translation, QNetworkAccessManager *nam, const QString &bookCode, int chapter);

    protected:
        QString chapterUrl(Translation* translation, const QString &bookCode, int chapterNo) const;
};

#endif
