#ifndef BLVCHAPTERREQUEST_H
#define BLVCHAPTERREQUEST_H

#include "ChapterRequest.h"

class BLVTranslation;

class BLVChapterRequest: public ChapterRequest
{
    public:
        BLVChapterRequest(
            BLVTranslation *translation,
            const QString& bookCode,
            int chapterNo,
            QNetworkReply *nreply
        );

    protected:
        virtual void onNReplyFinished();
};

#endif
