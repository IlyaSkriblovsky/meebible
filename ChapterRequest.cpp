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
    finished(QString::fromUtf8(_nreply->readAll()));
}


Translation* ChapterRequest::translation() const
{
    return _translation;
}

QString ChapterRequest::bookCode() const
{
    return _bookCode;
}

int ChapterRequest::chapterNo() const
{
    return _chapterNo;
}
