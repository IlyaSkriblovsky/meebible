#include "CCArabicParser.h"

#include <QNetworkAccessManager>
#include <QRegExp>
#include <QStringList>
#include <QTextCodec>

#include "EasyXml.h"
#include "ChapterRequest.h"
#include "Translation.h"


class CCArabicChapterRequest: public ChapterRequest
{
    public:
        CCArabicChapterRequest(
            Translation *translation,
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
                finished("");
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


CCArabicParser::CCArabicParser()
{
    // FIXME: This should be in DB, not in code
    _searchNames.insert("ge",   "Genesis");
    _searchNames.insert("ex",   "Exodus");
    _searchNames.insert("le",   "Leviticus");
    _searchNames.insert("nu",   "Numbers");
    _searchNames.insert("de",   "Deuteronomy");
    _searchNames.insert("jos",  "Joshua");
    _searchNames.insert("jg",   "Judges");
    _searchNames.insert("ru",   "Ruth");
    _searchNames.insert("1sa",  "1 Samuel");
    _searchNames.insert("2sa",  "2 Samuel");
    _searchNames.insert("1ki",  "1 Kings");
    _searchNames.insert("2ki",  "2 Kings");
    _searchNames.insert("1ch",  "1 Chronicles");
    _searchNames.insert("2ch",  "2 Chronicles");
    _searchNames.insert("ezr",  "Ezra");
    _searchNames.insert("ne",   "Nehemiah");
    _searchNames.insert("es",   "Esther");
    _searchNames.insert("job",  "Job");
    _searchNames.insert("ps",   "Psalms");
    _searchNames.insert("pr",   "Proverbs");
    _searchNames.insert("ec",   "Ecclesiastes");
    _searchNames.insert("ca",   "Song of Songs");
    _searchNames.insert("isa",  "Isaiah");
    _searchNames.insert("jer",  "Jeremiah");
    _searchNames.insert("la",   "Lamentations");
    _searchNames.insert("eze",  "Ezekiel");
    _searchNames.insert("da",   "Daniel");
    _searchNames.insert("ho",   "Hosea");
    _searchNames.insert("joe",  "Joel");
    _searchNames.insert("am",   "Amos");
    _searchNames.insert("ob",   "Obadiah");
    _searchNames.insert("jon",  "Jonah");
    _searchNames.insert("mic",  "Micah");
    _searchNames.insert("na",   "Nahum");
    _searchNames.insert("hab",  "Habakkuk");
    _searchNames.insert("zep",  "Zephaniah");
    _searchNames.insert("hag",  "Haggai");
    _searchNames.insert("zec",  "Zechariah");
    _searchNames.insert("mal",  "Malachi");
    _searchNames.insert("mt",   "Matthew");
    _searchNames.insert("mr",   "Mark");
    _searchNames.insert("lu",   "Luke");
    _searchNames.insert("joh",  "John");
    _searchNames.insert("ac",   "Acts");
    _searchNames.insert("ro",   "Romans");
    _searchNames.insert("1co",  "1 Corinthians");
    _searchNames.insert("2co",  "2 Corinthians");
    _searchNames.insert("ga",   "Galatians");
    _searchNames.insert("eph",  "Ephesians");
    _searchNames.insert("php",  "Philippians");
    _searchNames.insert("col",  "Colossians");
    _searchNames.insert("1th",  "1 Thessalonians");
    _searchNames.insert("2th",  "2 Thessalonians");
    _searchNames.insert("1ti",  "1 Timothy");
    _searchNames.insert("2ti",  "2 Timothy");
    _searchNames.insert("tit",  "Titus");
    _searchNames.insert("phm",  "Philemon");
    _searchNames.insert("heb",  "Hebrews");
    _searchNames.insert("jas",  "James");
    _searchNames.insert("1pe",  "1 Peter");
    _searchNames.insert("2pe",  "2 Peter");
    _searchNames.insert("1jo",  "1 John");
    _searchNames.insert("2jo",  "2 John");
    _searchNames.insert("3jo",  "3 John");
    _searchNames.insert("jude", "Jude");
    _searchNames.insert("re",   "Revelation");
}


ChapterRequest* CCArabicParser::requestChapter(MultiSource* source, Translation* translation, QNetworkAccessManager* nam, const QString& bookCode, int chapterNo)
{
    Q_UNUSED(source);

    QNetworkRequest request(QUrl(
        QString("http://copticchurch.net/cgibin/bible/index.php?r=%1+%2&version=SVD&showVN=1")
            .arg(_searchNames[bookCode])
            .arg(chapterNo)
    ));

    return new CCArabicChapterRequest(
        translation, bookCode, chapterNo,
        nam->get(request)
    );
}
