#include "BLVChapterRequest.h"

#include <QDomDocument>
#include <QRegExp>

#include "CCArabicTranslation.h"

#include "EasyXml.h"



BLVChapterRequest::BLVChapterRequest(
    CCArabicTranslation *translation,
    const QString& bookCode,
    int chapterNo,
    QNetworkReply *nreply
)
    : ChapterRequest(translation, bookCode, chapterNo, nreply)
{
}


void BLVChapterRequest::onNReplyFinished()
{
    if (error() != QNetworkReply::NoError)
    {
        finished("");
        return;
    }

    finished("");
}
