#ifndef BOCHAPTERREQUEST_H
#define BOCHAPTERREQUEST_H

#include "ChapterRequest.h"

class BORusTranslation;


class BOChapterRequest : public ChapterRequest
{
public:
    BOChapterRequest(
        BORusTranslation *translation,
        const QString& bookCode,
        int chapterNo,
        QNetworkReply *nreply
    );

protected:
    virtual void onNReplyFinished();
};

#endif // BOCHAPTERREQUEST_H
