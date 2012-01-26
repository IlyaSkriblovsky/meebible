#include "NWTParser.h"

#include <QNetworkAccessManager>
#include <QXmlQuery>
#include <QFile>

#include "ChapterRequest.h"
#include "Translation.h"
#include "Language.h"
#include "Paths.h"

class NWTChapterRequest: public ChapterRequest
{
    public:
        NWTChapterRequest(
            Translation* translation,
            const QString& bookCode,
            int chapterNo,
            QNetworkReply *nreply
        ): ChapterRequest(translation, bookCode, chapterNo, nreply)
        {
        }

    protected:
        static QString _xslt;

        static QString xslt()
        {
            if (_xslt.length() == 0)
            {
                QFile f(Paths::nwt_xslt());
                f.open(QIODevice::ReadOnly);
                _xslt = QString::fromUtf8(f.readAll());
            }

            return _xslt;
        }

        virtual void onNReplyFinished()
        {
            if (error() != QNetworkReply::NoError)
            {
                finished("");
                return;
            }


            QString content = QString::fromUtf8(_nreply->readAll());

        //  Unfortunately, I can't gracefully handle these diacritics in full-text search
        //    content = "<!DOCTYPE html ["
        //        "<!ENTITY nbsp \"&#160;\">"
        //        "<!ENTITY acute \"&#180;\">"
        //        "<!ENTITY middot \"&#183;\">"
        //        "]>\n"
        //    + content;

            QXmlQuery query(QXmlQuery::XSLT20);
            query.setFocus(content);

            query.setQuery(xslt());

            QString output;

            query.evaluateTo(&output);

            finished(output);
        }
};

QString NWTChapterRequest::_xslt;



NWTParser::NWTParser()
{
    _urlPrefix.insert("af",  "/af/bybel/");
    _urlPrefix.insert("al",  "/al/bible/");
    _urlPrefix.insert("bl",  "/bl/bible/");
    _urlPrefix.insert("cn",  "/cn/baibulo/");
    _urlPrefix.insert("chs", "/chs/bible/");
    _urlPrefix.insert("ch",  "/ch/bible/");
    _urlPrefix.insert("cw",  "/cw/baibolo/");
    _urlPrefix.insert("c",   "/c/biblija/");
    _urlPrefix.insert("b",   "/b/bible/");
    _urlPrefix.insert("d",   "/d/bibelen/");
    _urlPrefix.insert("o",   "/o/bijbel/");
    _urlPrefix.insert("e",   "/e/bible/");
    _urlPrefix.insert("fi",  "/fi/raamattu/");
    _urlPrefix.insert("f",   "/f/bible/");
    _urlPrefix.insert("ge",  "/ge/bible/");
    _urlPrefix.insert("x",   "/x/bibel/");
    _urlPrefix.insert("g",   "/g/bible/");
    _urlPrefix.insert("h",   "/h/biblia/");
    _urlPrefix.insert("in",  "/in/alkitab/");
    _urlPrefix.insert("i",   "/i/bibbia/");
    _urlPrefix.insert("j",   "/j/bible/");
    _urlPrefix.insert("ko",  "/ko/bible/");
    _urlPrefix.insert("li",  "/li/biblia/");
    _urlPrefix.insert("mg",  "/mg/baiboly/");
    _urlPrefix.insert("mt",  "/mt/bibbja/");
    _urlPrefix.insert("n",   "/n/bibelen/");
    _urlPrefix.insert("oss", "/oss/bible/");
    _urlPrefix.insert("p",   "/p/biblia/");
    _urlPrefix.insert("t",   "/t/biblia/");
    _urlPrefix.insert("m",   "/m/biblia/");
    _urlPrefix.insert("u",   "/u/bible/");
    _urlPrefix.insert("sb",  "/sb/bible/");
    _urlPrefix.insert("su",  "/su/bibele/");
    _urlPrefix.insert("sv",  "/sv/svetopismo/");
    _urlPrefix.insert("s",   "/s/biblia/");
    _urlPrefix.insert("sr",  "/sr/bijbel/");
    _urlPrefix.insert("sw",  "/sw/biblia/");
    _urlPrefix.insert("z",   "/z/bibeln/");
    _urlPrefix.insert("tn",  "/tn/baebele/");
    _urlPrefix.insert("tk",  "/tk/kutsalkitap/");
    _urlPrefix.insert("xo",  "/xo/ibhayibhile/");
    _urlPrefix.insert("zu",  "/zu/ibhayibheli/");
}


ChapterRequest* NWTParser::requestChapter(MultiSource *source, Translation *translation, QNetworkAccessManager *nam, const QString& bookCode, int chapterNo)
{
    Q_UNUSED(source);

    QString url = QString("http://www.watchtower.org%1%2/chapter_%3.htm")
                  .arg(_urlPrefix[translation->language()->code()])
                  .arg(bookCode)
                  .arg(chapterNo, 3, 10, QChar('0'));

    return new NWTChapterRequest(
        translation, bookCode, chapterNo,
        nam->get(QNetworkRequest(QUrl(url)))
    );
}
