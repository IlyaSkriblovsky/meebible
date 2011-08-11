#include "BORusTranslation.h"

#include <QDebug>
#include <QCoreApplication>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QNetworkAccessManager>
#include <QVariant>

#include "Language.h"
#include "BOChapterRequest.h"
#include "Paths.h"


//
// This class uses lazy loading of data. Do NOT use _bookCodes, _bookNames
// or _verseCounts directly anywhere (except in corresponding getters).
// Always use public getters which would load data from DB on first call.
//


BORusTranslation::BORusTranslation(const Language* language)
    : _language(language)
{
    _db = QSqlDatabase::addDatabase("QSQLITE", "bo");
    _db.setDatabaseName(Paths::boDB());
    if (! _db.open())
    {
        qDebug() << "Cannot open bo db";
        QCoreApplication::exit(1);
    }


}


const Language* BORusTranslation::language() const
{
    return _language;
}

QString BORusTranslation::code() const
{
    return "bo-sb";
}

QString BORusTranslation::name() const
{
    return QString::fromUtf8("Синодальный перевод");
}


QString BORusTranslation::bookName(const QString &bookCode) const
{
    if (_bookNames.size() == 0)
    {
        QSqlQuery select("SELECT bookCode, name FROM books", _db);
        select.exec();
        
        while (select.next())
        {
            _bookNames[select.value(0).toString()] = select.value(1).toString();
        }
    }

    qDebug() << _bookNames.value(bookCode);

    return _bookNames.value(bookCode);
}

QStringList BORusTranslation::bookCodes() const
{
    if (_bookCodes.size() == 0)
    {
        QSqlQuery select("SELECT bookCode FROM books ORDER BY no", _db);
        select.exec();

        while (select.next())
            _bookCodes.append(select.value(0).toString());
    }

    qDebug() << _bookCodes.size();

    return _bookCodes;
}


QString BORusTranslation::chapterUrl(const QString &bookCode, int chapterNo) const
{
    int no = bookCodes().indexOf(bookCode);
    if (no == -1)
        return "<illegal url>";

    no += 1;

    return QString("http://bibleonline.ru/bible/rus/%1/%2/")
            .arg(no, 2, 10, QChar('0'))
            .arg(chapterNo, 2, 10, QChar('0'));
}


ChapterRequest* BORusTranslation::requestChapter(QNetworkAccessManager* nam, const QString &bookCode, int chapterNo)
{
    return new BOChapterRequest(
        this, bookCode, chapterNo,
        nam->get(QNetworkRequest(QUrl(
            chapterUrl(bookCode, chapterNo)
        )))
    );
}


QList<int> BORusTranslation::verseCounts(const QString& bookCode) const
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
