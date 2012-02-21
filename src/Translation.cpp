#include "Translation.h"

#include <QHash>
#include <QByteArray>

#include <QNetworkAccessManager>
#include <QNetworkReply>

#include "ChapterRequest.h"
#include "Language.h"
#include "Cache.h"
#include "MetaInfoLoader.h"
#include "Paths.h"
#include "TranslationInfoParser.h"



Translation::Translation(
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
    QHash<int, QByteArray> roleNames;
    roleNames[Qt::DisplayRole] = "value";
    roleNames[BookCodeRole] = "bookCode";
    roleNames[BookNameRole] = "bookName";
    setRoleNames(roleNames);
}



int Translation::rowCount(const QModelIndex& index) const
{
    Q_UNUSED(index)

    return bookCodes().size();
}

QVariant Translation::data(const QModelIndex& index, int role) const
{
    QStringList codes = bookCodes();

    if (index.row() < 0  ||  index.row() >= codes.size())
        return QVariant();

    switch (role)
    {
        case BookCodeRole: return codes.at(index.row());

        case Qt::DisplayRole:
        case BookNameRole:
            return bookName(codes.at(index.row()));
    }

    return QVariant();
}



QString Translation::bookName(const QString& bookCode) const
{
    if (_books.contains(bookCode))
        return _books.value(bookCode).name;

    return "<unknown>";
}

QStringList Translation::bookCodes() const
{
    return _bookCodes;
}

QList<int> Translation::verseCounts(const QString& bookCode) const
{
    if (_books.contains(bookCode))
        return _books.value(bookCode).chapterSize;

    return QList<int>();
}



int Translation::chaptersInBook(const QString& bookCode) const
{
    return verseCounts(bookCode).size();
}

int Translation::versesInChapter(const QString& bookCode, int chapterNo) const
{
    return verseCounts(bookCode).at(chapterNo - 1);
}



QString Translation::bookCodeAt(int row) const
{
    QStringList codes = bookCodes();

    if (row >= 0 && row < codes.size())
        return codes.at(row);

    return "";
}



bool Translation::hasBook(const QString& bookCode) const
{
    return bookCodes().indexOf(bookCode) != -1;
}




void Translation::reload(bool useCache)
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

void Translation::translationXMLReceived()
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

void Translation::loadFromXML(const QString& xml)
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

void Translation::setLoading(bool loading)
{
    if (_loading != loading)
    {
        _loading = loading;
        loadingChanged();
    }
}



void Translation::addBookInfo(const Translation::BookInfo& bookInfo)
{
    _books.insert(bookInfo.code, bookInfo);
    _bookCodes.append(bookInfo.code);
}



ChapterRequest* Translation::requestChapter(QNetworkAccessManager* nam, const Place& place)
{
    QString url = QString("%1?trans=%2&lang=%3&book=%4&chapter=%5")
                .arg(Paths::wsUrl("chapter").toString())
                .arg(_code)
                .arg(_language->code())
                .arg(place.bookCode())
                .arg(place.chapterNo());

    return new ChapterRequest(
        this, place,
        nam->get(QNetworkRequest(QUrl(
            url
        )))
    );
}
