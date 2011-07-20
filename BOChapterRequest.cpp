#include "BOChapterRequest.h"

#include <QByteArray>
#include <QXmlQuery>
#include <QBuffer>
#include <QFile>

#include <QDomDocument>

#include "BORusTranslation.h"
#include "EasyXml.h"


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
    QString content = QString::fromUtf8(_nreply->readAll());

    EasyElement* body = tag("body")->append(
        tag("h3")
        ->text(
            QString("%1 %2:1-%3")
                .arg(translation()->bookName(bookCode()))
                .arg(chapterNo())
                .arg(translation()->versesInChapter(bookCode(), chapterNo()))
        )
    );

    QRegExp re("<span id=\"v(\\d+)\" class=\"v\"><sup>[^<]*</sup>([^<]*)<br /></span>");

    int pos = 0;
    while ((pos = re.indexIn(content, pos)) != -1)
    {
        pos += re.matchedLength();

        body->append(
            tag("div")
            ->attr("class", "par")
            ->append(
                tag("div")
                ->attr("class", "verse")
                ->attr("verse", re.cap(1))
                ->append(
                    tag("div")
                    ->attr("class", "verse-label")
                    ->text(re.cap(1))
                )
                ->text(re.cap(2))
            )
        );
    }


    QDomDocument doc;

    tag("html")
    ->append(
        tag("head")
        ->append(
            tag("meta")
            ->attr("http-equiv", "Content-Type")
            ->attr("content", "text/html; charset=utf-8")
        )
        ->append(
            tag("style")
            ->text(
                ".par { margin-left: .8em; text-indent: -1em; }"
            )
        )
    )
    ->append(body)
    ->appendTo(doc)
    ->del();


    finished(doc.toString());
}
