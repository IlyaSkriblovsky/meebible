#include "KJBOSource.h"

#include <QDebug>

#include <QNetworkAccessManager>
#include <QRegExp>
#include <QStringList>

#include "EasyXml.h"
#include "ChapterRequest.h"
#include "SimpleTranslation.h"


class KJBOChapterRequest: public ChapterRequest
{
    public:
        KJBOChapterRequest(
            SimpleTranslation *translation,
            const QString& bookCode,
            int chapterNo,
            QNetworkReply *nreply
        ) : ChapterRequest(translation, bookCode, chapterNo, nreply)
        {
        }

    protected:
        virtual void onNReplyFinished()
        {
            if (error() != QNetworkReply::NoError)
            {
                finished("HTTP Error");
                return;
            }

            QString content = QString::fromUtf8(_nreply->readAll());

            EasyElement* body = tag("body");

            QRegExp re("<a href=\"http://www.kingjamesbibleonline.org/[^\"]*-(\\d+)/\" title='View more translations[^']*'>(.*)</a></p>");
            re.setMinimal(true);

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
                        ".verse-label { min-width: 2.5ex; }"
                    )
                )
            )
            ->append(body)
            ->appendTo(doc)
            ->del();


            QString html = doc.toString();

            finished(html);
        }
};



KJBOSource::KJBOSource()
    : SimpleSource("e")
{
}

KJBOSource::~KJBOSource()
{
}

QString KJBOSource::chapterUrl(SimpleTranslation *translation, const QString& bookCode, int chapterNo) const
{
    return QString("http://www.kingjamesbibleonline.org/%1-Chapter-%2/")
        .arg(translation->bookName(bookCode).replace(" ", "-"))
        .arg(chapterNo);
}

ChapterRequest* KJBOSource::requestChapter(SimpleTranslation *translation, QNetworkAccessManager *nam, const QString& bookCode, int chapterNo)
{
    QString url = chapterUrl(translation, bookCode, chapterNo);
    qDebug() << url;
    return new KJBOChapterRequest(
        translation, bookCode, chapterNo,
        nam->get(QNetworkRequest(QUrl(
            url
        )))
    );
}
