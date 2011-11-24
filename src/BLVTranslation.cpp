#include "BLVTranslation.h"

#include <QCoreApplication>
#include <QSqlQuery>
#include <QByteArray>

#include <QDebug>

#include "Paths.h"
#include "BLVChapterRequest.h"

//
// This class uses lazy loading of data. Do NOT use _bookCodes, _bookNames
// or _verseCounts directly anywhere (except in corresponding getters).
// Always use public getters which would load data from DB on first call.
//

BLVTranslation::BLVTranslation(const Language* language)
    : _language(language)
{
    _db = QSqlDatabase::addDatabase("QSQLITE", "blv");
    _db.setDatabaseName(Paths::blvDB());
    if (! _db.open())
    {
        qCritical() << "Cannot open blv db";
        QCoreApplication::exit(1);
    }
}


QString BLVTranslation::bookName(const QString &bookCode) const
{
    if (_bookNames.size() == 0)
    {
        QSqlQuery select("SELECT bookCode, name FROM books", _db);
        select.exec();

        while (select.next())
            _bookNames[select.value(0).toString()] = select.value(1).toString();
    }

    return _bookNames.value(bookCode);
}

QStringList BLVTranslation::bookCodes() const
{
    if (_bookCodes.size() == 0)
    {
        QSqlQuery select("SELECT bookCode FROM books ORDER BY no", _db);
        select.exec();

        while (select.next())
            _bookCodes.append(select.value(0).toString());
    }

    return _bookCodes;
}


ChapterRequest* BLVTranslation::requestChapter(QNetworkAccessManager* nam, const QString& bookCode, int chapterNo)
{
    QNetworkRequest request(QUrl("http://bibele.lv/bibele/bibele.php"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    QByteArray args = QString("book=%1&chapter=%2")
        .arg(bookCodes().indexOf(bookCode) + 1)
        .arg(chapterNo)
        .toAscii();

    // FIXME
    return new BLVChapterRequest(
        this, bookCode, chapterNo,
        nam->post(request, args)
    );
}


QList<int> BLVTranslation::verseCounts(const QString& bookCode) const
{
    if (_verseCounts.size() == 0)
    {
        QStringList bcs = bookCodes();
        for (int i = 0; i < bcs.size(); i++)
        {
            QString bookCode = bcs[i];

            QSqlQuery select("SELECT verses FROM chapterSize WHERE bookCode = ? ORDER BY chapterNo", _db);
            select.addBindValue(bookCode);
            select.exec();

            QList<int> verseCounts;
            while (select.next())
                verseCounts.append(select.value(0).toInt());

            _verseCounts[bookCode] = verseCounts;
        }
    }

    return _verseCounts.value(bookCode);
}
