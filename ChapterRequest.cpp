#include "ChapterRequest.h"

#include <QNetworkReply>


#include "Translation.h"


ChapterRequest::ChapterRequest(
    Translation* translation,
    const QString& bookCode,
    int chapterNo,
    QNetworkReply* nreply
):
    QObject(translation),
    _nreply(nreply),
    _translation(translation),
    _bookCode(bookCode),
    _chapterNo(chapterNo)
{
    _nreply->setParent(this);

    connect(_nreply, SIGNAL(finished()), this, SLOT(onNReplyFinished()));
}

void ChapterRequest::onNReplyFinished()
{
    finished(_nreply->readAll());
}
