#ifndef CCARABICCHAPTERREQUEST_H
#define CCARABICCHAPTERREQUEST_H

#include "ChapterRequest.h"

class CCArabicTranslation;

class CCArabicChapterRequest: public ChapterRequest
{
    public:
        CCArabicChapterRequest(
            CCArabicTranslation *translation,
            const QString& bookCode,
            int chapterNo,
            QNetworkReply *nreply
        );

    protected:
        virtual void onNReplyFinished();
};

#endif
