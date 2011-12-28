#include "BLVSource2.h"

#include <QNetworkAccessManager>
#include <QRegExp>
#include <QStringList>

#include "EasyXml.h"
#include "ChapterRequest.h"
#include "SimpleTranslation.h"


class BLVChapterRequest2: public ChapterRequest
{
    public:
        BLVChapterRequest2(
            SimpleTranslation *translation,
            const QString& bookCode,
            int chapterNo,
            QNetworkReply *nreply
        )
            : ChapterRequest(translation, bookCode, chapterNo, nreply)
        {
        }

    protected:
        virtual void onNReplyFinished()
        {
            if (error() != QNetworkReply::NoError)
            {
                finished("");
                return;
            }

            QString content = QString::fromUtf8(_nreply->readAll());

            QRegExp re_payload = QRegExp("<SPAN CLASS='normal'>(.*)</SPAN>");
            re_payload.setMinimal(true);

            QString payload;

            if (re_payload.indexIn(content, 0) == -1)
                payload = "[BLV: not found?]";


            payload = re_payload.cap(1);

            QRegExp re_verse_no = QRegExp("^\\d+ +");

            QStringList verses = payload.split("<br>");
            for (int i = 0; i < verses.size(); )
            {
                if (verses.at(i).trimmed().length() == 0)
                    verses.removeAt(i);
                else
                {
                    verses[i] = verses.at(i).trimmed().replace(re_verse_no, "");
                    i++;
                }
            }

            if (bookCode() == "ac"  &&  chapterNo() == 24)
                verses.insert(6, QString::fromUtf8("———"));
            if (bookCode() == "ac"  &&  chapterNo() == 28)
                verses.insert(28, QString::fromUtf8("———"));


            EasyElement* body = tag("body");
            for (int i = 0; i < verses.size(); i++)
            {
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
                        ".par { margin-left: .8em; text-indent: -1em; }"
                        ".verse-label { min-width: 2.5ex; }"
                    )
                )
            )
            ->append(body)
            ->appendTo(doc)
            ->del();

            finished(doc.toString());
        }
};



BLVSource2::BLVSource2()
            :SimpleSource("lv")
{
}

BLVSource2::~BLVSource2()
{
}



ChapterRequest* BLVSource2::requestChapter(SimpleTranslation* translation, QNetworkAccessManager *nam, const QString& bookCode, int chapterNo)
{
    QNetworkRequest request(QUrl("http://bibele.lv/bibele/bibele.php"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    QByteArray args = QString("book=%1&chapter=%2")
        .arg(translation->bookCodes().indexOf(bookCode) + 1)
        .arg(chapterNo)
        .toAscii();

    // FIXME
    return new BLVChapterRequest2(
        translation, bookCode, chapterNo,
        nam->post(request, args)
    );
}
