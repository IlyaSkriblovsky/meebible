#include "BLVTranslation.h"

#include <QDebug>

//
// This class uses lazy loading of data. Do NOT use _bookCodes, _bookNames
// or _verseCounts directly anywhere (except in corresponding getters).
// Always use public getters which would load data from DB on first call.
//

BLVTranslation::BLVTranslation(const Language* language)
    : _language(language)
{
}


QString BORusTranslation::bookName(const QString &bookCode) const
{
    // FIXME
    return "unimplemented";
}

QStringList BORusTranslation::bookCodes() const
{
    // FIXME
    return QStringList();
}


ChapterRequest* BLVTranslation::requestChapter(QNetworkAccessManager* nam, const QString bookCode, int chapterNo)
{
    // FIXME
    return new BLVChapterRequest(
        this, bookCode, chapterNo,
        nam->get(QNetworkRequest(QUrl(
            "http://ya.ru"
        )))
    );
}


QList<int> BLVTranslation::verseCounts(const QString& bookCode) const
{
    // FIXME
    return QList<int>();
}
