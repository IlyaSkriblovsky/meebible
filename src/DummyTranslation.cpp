#include "DummyTranslation.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>

#include "Paths.h"
#include "TranslationInfoParser.h"
#include "ChapterRequest.h"
#include "MetaInfoLoader.h"




class DummyChapterRequest: public ChapterRequest
{
    public:
        DummyChapterRequest(
            DummyTranslation *translation,
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
            finished(content);
        }
};




DummyTranslation::DummyTranslation(
    const QString& code,
    Language* language,
    const QString& name,
    const QString& sourceUrl,
    const QString& copyright
)
    : _code(code), _language(language), _name(name),
      _sourceUrl(sourceUrl), _copyright(copyright)
{
}

DummyTranslation::~DummyTranslation()
{
}



QString DummyTranslation::bookName(const QString& bookCode) const
{
    if (_books.contains(bookCode))
        return _books.value(bookCode).name;

    return "<unknown>";
}

QStringList DummyTranslation::bookCodes() const
{
    return _bookCodes;
}

QList<int> DummyTranslation::verseCounts(const QString& bookCode) const
{
    if (_books.contains(bookCode))
        return _books.value(bookCode).chapterSize;

    return QList<int>();
}



void DummyTranslation::reload()
{
    MetaInfoLoader::instance()->loadTranslationInfo(this);
}

void DummyTranslation::loadFromXML(const QString& xml)
{
    beginResetModel();
    _books.clear();
    _bookCodes.clear();

    TranslationInfoParser parser(this);
    QXmlSimpleReader reader;
    reader.setContentHandler(&parser);
    reader.setErrorHandler(&parser);

    QXmlInputSource source;
    source.setData(xml);

    reader.parse(source);

    endResetModel();

    loaded();
}



void DummyTranslation::addBookInfo(const DummyTranslation::BookInfo& bookInfo)
{
    _books.insert(bookInfo.code, bookInfo);
    _bookCodes.append(bookInfo.code);
}



ChapterRequest* DummyTranslation::requestChapter(QNetworkAccessManager* nam, const QString& bookCode, int chapterNo)
{
    QString url = QString("%1?trans=%2&lang=%3&book=%4&chapter=%5")
                .arg(Paths::wsUrl("chapter").toString())
                .arg(_code)
                .arg(_language->code())
                .arg(bookCode)
                .arg(chapterNo);

    qDebug() << url;
    return new DummyChapterRequest(
        this, bookCode, chapterNo,
        nam->get(QNetworkRequest(QUrl(
            url
        )))
    );
}
