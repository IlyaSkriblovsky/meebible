#include "NWTChapterRequest.h"

#include <QByteArray>

#include <QXmlQuery>
#include <QFile>
#include <QBuffer>

#include "NWTranslation.h"
#include "Paths.h"


QString NWTChapterRequest::_xslt;


NWTChapterRequest::NWTChapterRequest(
    NWTranslation *translation,
    const QString& bookCode,
    int chapterNo,
    QNetworkReply *nreply
)
    : ChapterRequest(translation, bookCode, chapterNo, nreply)
{
}


QString NWTChapterRequest::xslt()
{
    if (_xslt.length() == 0)
    {
        QFile f(Paths::nwt_xslt());
        f.open(QIODevice::ReadOnly);
        _xslt = QString::fromUtf8(f.readAll());
    }

    return _xslt;
}


void NWTChapterRequest::onNReplyFinished()
{
    if (error() != QNetworkReply::NoError)
    {
        finished("");
        return;
    }


    QByteArray content = _nreply->readAll();

    QXmlQuery query(QXmlQuery::XSLT20);
    query.setFocus(QString::fromUtf8(content));

    query.setQuery(xslt());

    QString output;

    query.evaluateTo(&output);

    finished(output);
}
