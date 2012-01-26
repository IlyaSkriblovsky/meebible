#include "MultiTranslation.h"

#include "MultiSource.h"


MultiTranslation::MultiTranslation(
    MultiSource* source,
    Language* language,
    const QString& parser,
    const QString& code,
    const QString& name,
    const QString& sourceUrl,
    const QString& copyright,
    bool rtl
)
    : _source(source), _language(language), _parser(parser),
      _code(code), _name(name), _sourceUrl(sourceUrl),
      _copyright(copyright), _rtl(rtl)
{
}


QString MultiTranslation::bookName(const QString& bookCode) const
{
    return _source->bookName(this, bookCode);
}

QStringList MultiTranslation::bookCodes() const
{
    return _source->bookCodes(this);
}

QList<int> MultiTranslation::verseCounts(const QString& bookCode) const
{
    return _source->verseCounts(this, bookCode);
}

ChapterRequest* MultiTranslation::requestChapter(QNetworkAccessManager *nam, const QString& bookCode, int chapterNo)
{
    return _source->requestChapter(this, nam, bookCode, chapterNo);
}
