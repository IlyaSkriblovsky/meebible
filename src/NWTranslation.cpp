#include "NWTranslation.h"

#include "NWTSource.h"
#include "Language.h"
#include "NWTChapterRequest.h"


NWTranslation::NWTranslation(const NWTSource* source, const Language *lang, const QString& urlPrefix)
    : _source(source), _lang(lang), _urlPrefix(urlPrefix)
{
}

NWTranslation::~NWTranslation()
{
}


const Language* NWTranslation::language() const
{
    return _lang;
}


QString NWTranslation::code() const
{
    return "nwt";
}


QString NWTranslation::name() const
{
    return tr("New World Translation");
}

QString NWTranslation::sourceUrl() const
{
    return QString("http://www.watchtower.org%1").arg(_urlPrefix);
}

QString NWTranslation::copyright() const
{
    return QString::fromUtf8("© Watch Tower Bible and Tract Society of Pennsylvania");
}


QString NWTranslation::bookName(const QString &bookCode) const
{
    if (_bookNames.size() == 0)
        _bookNames = _source->bookNames(_lang);

    return _bookNames.value(bookCode);
}

QStringList NWTranslation::bookCodes() const
{
    return _source->bookCodes();
}


QList<int> NWTranslation::verseCounts(const QString& bookCode) const
{
    return _source->verseCounts(bookCode);
}


QString NWTranslation::chapterUrl(const QString& bookCode, int chapterNo) const
{
    return QString("http://www.watchtower.org%1%2/chapter_%3.htm")
            .arg(_urlPrefix)
            .arg(bookCode)
            .arg(chapterNo, 3, 10, QChar('0'));
}


ChapterRequest* NWTranslation::requestChapter(QNetworkAccessManager* nam, const QString &bookCode, int chapterNo)
{
    return new NWTChapterRequest(
        this, bookCode, chapterNo,
        nam->get(QNetworkRequest(QUrl(
            chapterUrl(bookCode, chapterNo)
        )))
    );
}
