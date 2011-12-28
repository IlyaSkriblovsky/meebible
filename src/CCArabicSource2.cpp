#include "CCArabicSource2.h"

#include <QNetworkAccessManager>
#include <QRegExp>
#include <QStringList>
#include <QTextCodec>
#include <QSqlQuery>

#include "EasyXml.h"
#include "ChapterRequest.h"
#include "SimpleTranslation.h"


class CCArabicChapterRequest: public ChapterRequest
{
    public:
        CCArabicChapterRequest(
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
};



class CCArabicTranslation: public SimpleTranslation
{
    public:
        CCArabicTranslation(SimpleSource *source, const Language* language)
            :SimpleTranslation(source, language)
        {
        }

        QString bookSearchName(const QString &bookCode) const
        {
            if (_bookSearchNames.size() == 0)
            {
                QSqlQuery select("SELECT bookCode, searchName FROM books", _db);
                select.exec();

                while (select.next())
                    _bookSearchNames[select.value(0).toString()] = select.value(1).toString();
            }

            return _bookSearchNames.value(bookCode);
        }

    private:
        mutable QMap<QString, QString> _bookSearchNames;
};



CCArabicSource2::CCArabicSource2()
            :SimpleSource("a")
{
}

CCArabicSource2::~CCArabicSource2()
{
}


Translation* CCArabicSource2::createTranslation(const Language* language)
{
    return new CCArabicTranslation(this, language);
}


ChapterRequest* CCArabicSource2::requestChapter(SimpleTranslation* translation, QNetworkAccessManager* nam, const QString& bookCode, int chapterNo)
{
    QNetworkRequest request(QUrl(
        QString("http://copticchurch.net/cgibin/bible/index.php?r=%1+%2&version=SVD&showVN=1")
            .arg(dynamic_cast<CCArabicTranslation*>(translation)->bookSearchName(bookCode).replace(" ", "+"))
            .arg(chapterNo)
    ));

    return new CCArabicChapterRequest(
        translation, bookCode, chapterNo,
        nam->get(request)
    );
}
