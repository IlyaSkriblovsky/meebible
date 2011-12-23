#include "CCArabicChapterRequest.h"

#include <QDomDocument>
#include <QRegExp>
#include <QTextCodec>

#include "CCArabicTranslation.h"

#include "EasyXml.h"



CCArabicChapterRequest::CCArabicChapterRequest(
    CCArabicTranslation *translation,
    const QString& bookCode,
    int chapterNo,
    QNetworkReply *nreply
)
    : ChapterRequest(translation, bookCode, chapterNo, nreply)
{
}


void CCArabicChapterRequest::onNReplyFinished()
{
    if (error() != QNetworkReply::NoError)
    {
        finished("");
        return;
    }

    QByteArray content_bin = _nreply->readAll();

    QTextCodec *codec = QTextCodec::codecForName("WINDOWS-1256");
    QString content = codec->toUnicode(content_bin);

    QRegExp re_payload = QRegExp("<span class=\"arabictext\">([^<]*)<br>");
    re_payload.setMinimal(true);

    if (re_payload.indexIn(content, 0) == -1)
    {
        finished("[CCArabic: not found?]");
        return;
    }

    QString payload = re_payload.cap(1);


    QStringList verses = payload.trimmed().split("\n");

    QRegExp re_verse_no = QRegExp("^\\d+ +");


    EasyElement* body = tag("body")->attr("dir", "rtl");
    for (int i = 0; i < verses.size(); i++)
    {
        verses[i] = verses.at(i).trimmed().replace(re_verse_no, "");

        body->append(
            tag("div")
            ->attr("class", "par")
            ->append(
                tag("div")
                ->attr("class", "verse")
                ->attr("verse", QString::number(i+1))
                ->append(
                    tag("div")
                    ->attr("class", "verse-label")
                    ->text(QString::number(i+1))
                )
                ->text(verses.at(i))
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
                ".par { margin-right: .8em; text-indent: -1em; }"
                ".verse-label { min-width: 2.5ex; }"
            )
        )
    )
    ->append(body)
    ->appendTo(doc)
    ->del();

    finished(doc.toString());
}
