#include "BOChapterRequest.h"

#include <QByteArray>
#include <QXmlQuery>
#include <QBuffer>
#include <QFile>

#include "BORusTranslation.h"

BOChapterRequest::BOChapterRequest(
    BORusTranslation *translation,
    const QString& bookCode,
    int chapterNo,
    QNetworkReply *nreply
)
    : ChapterRequest(translation, bookCode, chapterNo, nreply)
{
}


void BOChapterRequest::onNReplyFinished()
{
    QByteArray content = _nreply->readAll();

    QXmlQuery query(QXmlQuery::XSLT20);
    query.setFocus(QString::fromUtf8(content));

    QFile xslt(":/bo.xslt");
    xslt.open(QIODevice::ReadOnly);
    query.setQuery(&xslt);

    QBuffer buffer;
    buffer.open(QIODevice::WriteOnly);

    query.evaluateTo(&buffer);

    buffer.close();

    finished(buffer.buffer());
}
