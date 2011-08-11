#ifndef NWTCHAPTERREQUEST_H
#define NWTCHAPTERREQUEST_H

#include "ChapterRequest.h"

class NWTranslation;


class NWTChapterRequest: public ChapterRequest
{
public:
    NWTChapterRequest(
        NWTranslation *translation,
        const QString& bookCode,
        int chapterNo,
        QNetworkReply *nreply
    );

protected:
    virtual void onNReplyFinished();


    static QString _xslt;
    static QString xslt();
};

#endif // NWTCHAPTERREQUEST_H
