#include "CCArabicTranslation.h"

#include <QCoreApplication>
#include <QSqlQuery>
#include <QByteArray>

#include <QDebug>

#include "Paths.h"
#include "CCArabicChapterRequest.h"

//
// This class uses lazy loading of data. Do NOT use _bookCodes, _bookNames
// or _verseCounts directly anywhere (except in corresponding getters).
// Always use public getters which would load data from DB on first call.
//

CCArabicTranslation::CCArabicTranslation(const Language* language)
    : _language(language)
{
    _db = QSqlDatabase::addDatabase("QSQLITE", "ccarabic");
    _db.setDatabaseName(Paths::ccarabicDB());
    if (! _db.open())
    {
        qCritical() << "Cannot open ccarabic db";
        QCoreApplication::exit(1);
    }
}


QString CCArabicTranslation::bookName(const QString &bookCode) const
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

QStringList CCArabicTranslation::bookCodes() const
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


ChapterRequest* CCArabicTranslation::requestChapter(QNetworkAccessManager* nam, const QString& bookCode, int chapterNo)
{
    QNetworkRequest request(QUrl(
        QString("http://copticchurch.net/cgibin/bible/index.php?r=%1+%2&version=SVD&showVN=1")
            .arg(bookSearchName(bookCode).replace(" ", "+"))
            .arg(chapterNo)
    ));

    return new CCArabicChapterRequest(
        this, bookCode, chapterNo,
        nam->get(request)
    );
}


QList<int> CCArabicTranslation::verseCounts(const QString& bookCode) const
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

QString CCArabicTranslation::bookSearchName(const QString &bookCode) const
{
    if (_bookSearchNames.size() == 0)
    {
        QSqlQuery select("SELECT bookCode, searchName FROM books", _db);
        select.exec();

        while (select.next())
            _bookSearchNames[select.value(0).toString()] = select.value(1).toString();
    }

    return _bookSearchNames.value(bookCode);
}
