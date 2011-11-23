#include "BLVChapterRequest.h"

BLVChapterRequest::BLVChapterRequest(
    BLVTranslation *translation,
    const QString& bookCode,
    int chapterNo,
    QNetworkReply *nreply
)
    : ChapterRequest(translation, bookCode, chapterNo, reply)
{
}
