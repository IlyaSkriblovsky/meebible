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


QString NWTranslation::name() const
{
    return "New World Translation";
}


QString NWTranslation::bookName(const QString &bookCode) const
{
    return _source->bookName(_lang, bookCode);
}

QList<QString> NWTranslation::bookCodes() const
{
    return _source->bookCodes();
}

int NWTranslation::chaptersInBook(const QString &bookCode) const
{
    return _source->chaptersInBook(bookCode);
}

int NWTranslation::versesInChapter(const QString &bookCode, int chapterNo) const
{
    return _source->versesInChapter(bookCode, chapterNo);
}


QString NWTranslation::chapterUrl(const QString& bookCode, int chapterNo) const
{
    return QString("http://watchtower.org/%1/%2/chapter_%3.htm")
            .arg(_urlPrefix)
            .arg(bookCode)
            .arg(chapterNo, 3, 10, QChar('0'));
}


ChapterRequest* NWTranslation::requestChapter(const QString &bookCode, int chapterNo)
{
    return new NWTChapterRequest(
        this, bookCode, chapterNo,
        _source->requestChapter(
            chapterUrl(bookCode, chapterNo)
        )
    );
}
