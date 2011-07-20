#include "BOChapterRequest.h"

#include <QByteArray>
#include <QXmlQuery>
#include <QBuffer>
#include <QFile>

#include <QDomDocument>

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
//    QByteArray content = _nreply->readAll();
//
//    QXmlQuery query(QXmlQuery::XSLT20);
//    query.setFocus(QString::fromUtf8(content));
//
//    QFile xslt(":/bo.xslt");
//    xslt.open(QIODevice::ReadOnly);
//    query.setQuery(&xslt);
//
//    QBuffer buffer;
//    buffer.open(QIODevice::WriteOnly);
//
//    query.evaluateTo(&buffer);
//
//    buffer.close();
//
//    finished(buffer.buffer());

    QString content = QString::fromUtf8(_nreply->readAll());

    qDebug() << content;
    qDebug() << "=============";

    QRegExp re("<span id=\"v(\\d+)\" class=\"v\"><sup>[^<]*</sup>([^<]*)<br /></span>");

    int pos = 0;
    while ((pos = re.indexIn(content, pos)) != -1)
    {
        qDebug() << re.cap(1) << re.cap(2);
        pos += re.matchedLength();
    }


    QDomDocument doc;
    QDomElement html = doc.createElement("html");
    doc.appendChild(html);

    QDomElement body = doc.createElement("body");
    html.appendChild(body);

    QDomElement h3 = doc.createElement("h3");
    body.appendChild(h3);

    QDomText chapter = doc.createTextNode(
        QString("%1 %2:1-%3")
            .arg(translation()->bookName(bookCode()))
            .arg(chapterNo())
            .arg(translation()->versesInChapter(bookCode(), chapterNo()))
    );

    h3.appendChild(chapter);


    finished(doc.toString().toUtf8());
}
