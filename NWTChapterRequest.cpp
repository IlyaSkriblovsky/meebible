#include "NWTChapterRequest.h"

#include <QByteArray>

#include <QXmlQuery>
#include <QFile>
#include <QBuffer>

#include "NWTranslation.h"


NWTChapterRequest::NWTChapterRequest(
    NWTranslation *translation,
    const QString& bookCode,
    int chapterNo,
    QNetworkReply *nreply
)
    : ChapterRequest(translation, bookCode, chapterNo, nreply)
{
}


void NWTChapterRequest::onNReplyFinished()
{
    QByteArray content = _nreply->readAll();

    QXmlQuery query(QXmlQuery::XSLT20);
    query.setFocus(QString::fromUtf8(content));

    QFile xslt(":/nwt.xslt");
    xslt.open(QIODevice::ReadOnly);
    query.setQuery(&xslt);

    QBuffer buffer;
    buffer.open(QIODevice::WriteOnly);

    query.evaluateTo(&buffer);

    buffer.close();


    finished(buffer.buffer());
}
