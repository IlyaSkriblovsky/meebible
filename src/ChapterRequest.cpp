#include "ChapterRequest.h"

#include <QNetworkReply>


#include "Translation.h"


ChapterRequest::ChapterRequest(
    Translation* translation,
    const Place& place,
    QNetworkReply* nreply
):
    QObject(translation),
    _nreply(nreply),
    _translation(translation),
    _place(place)
{
    _nreply->setParent(this);

    connect(_nreply, SIGNAL(finished()), this, SLOT(onNReplyFinished()));
}

void ChapterRequest::onNReplyFinished()
{
    if (error() != QNetworkReply::NoError)
    {
        finished("HTTP Error");
        return;
    }

    QString content = QString::fromUtf8(_nreply->readAll());
    finished(content);
}


Translation* ChapterRequest::translation() const
{
    return _translation;
}

Place ChapterRequest::place() const
{
    return _place;
}

QNetworkReply::NetworkError ChapterRequest::error() const
{
    return _nreply->error();
}
