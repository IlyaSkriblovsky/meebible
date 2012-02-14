#include "DummyTranslation.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>

#include "Paths.h"
#include "TranslationInfoParser.h"
#include "ChapterRequest.h"
#include "MetaInfoLoader.h"
#include "Cache.h"



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
    const QString& copyright,
    bool rtl
)
    : _code(code), _language(language), _name(name),
      _sourceUrl(sourceUrl), _copyright(copyright),
      _rtl(rtl), _loading(false)
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



void DummyTranslation::reload(bool useCache)
{
    if (useCache)
    {
        QString xmlName = QString("trans_%1_%2").arg(_code, _language->code());
        if (Cache::instance()->hasXML(xmlName))
        {
            loadFromXML(Cache::instance()->loadXML(xmlName));
            loadingFinished();
            loadedChanged();
            return;
        }
    }

    QNetworkReply* reply = MetaInfoLoader::instance()->nam()->get(QNetworkRequest(
        QString("%1?trans=%2&lang=%3")
            .arg(Paths::wsUrl("translation").toString())
            .arg(_code)
            .arg(_language->code())
    ));
    connect(reply, SIGNAL(finished()), this, SLOT(translationXMLReceived()));

    setLoading(true);
}

void DummyTranslation::translationXMLReceived()
{
    QNetworkReply* reply = dynamic_cast<QNetworkReply*>(sender());
    reply->deleteLater();

    if (reply->error() != QNetworkReply::NoError)
    {
        setLoading(false);
        loadingError();
        return;
    }

    QString content = QString::fromUtf8(reply->readAll());

    loadFromXML(content);

    QString xmlName = QString("trans_%1_%2").arg(_code, _language->code());
    Cache::instance()->saveXML(xmlName, content);
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

    setLoading(false);
    loadingFinished();
    loadedChanged();
}

void DummyTranslation::setLoading(bool loading)
{
    if (_loading != loading)
    {
        _loading = loading;
        loadingChanged();
    }
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

    return new DummyChapterRequest(
        this, bookCode, chapterNo,
        nam->get(QNetworkRequest(QUrl(
            url
        )))
    );
}
