#include "NWTSource.h"

#include <QtCore>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QNetworkReply>


#include "Languages.h"
#include "NWTranslation.h"


//
// This class uses lazy loading of data. Do NOT use _bookCodes, _bookNames
// or _verseCounts directly anywhere (except in corresponding getters).
// Always use public getters which would load data from DB on first call.
//


NWTSource::NWTSource()
{
    _db = QSqlDatabase::addDatabase("QSQLITE", "nwt");
    _db.setDatabaseName("../MeeBible2/nwt.sqlite");
    if (! _db.open())
    {
        qDebug() << "Cannot open nwt db";
        QCoreApplication::exit(1);
    }
}


void NWTSource::addTranslationsToList(Languages* languages)
{
    QSqlQuery select("SELECT langCode, urlPrefix FROM langs", _db);

    while (select.next())
    {
        Language *lang = languages->langByCode(select.value(0).toString());
        lang->addTranslation(
            new NWTranslation(
                this,
                lang,
                select.value(1).toString()
            )
        );
    }
}


QStringList NWTSource::bookCodes() const
{
    if (_bookCodes.size() == 0)
    {
        QSqlQuery select("SELECT bookCode FROM books ORDER BY no", _db);
        while (select.next())
            _bookCodes.append(select.value(0).toString());
    }

    return _bookCodes;
}



QString NWTSource::bookName(const Language *lang, const QString &bookCode) const
{
    QSqlQuery select(_db);
    select.prepare("SELECT name FROM bookNames WHERE bookCode=? AND langCode=?");
    select.bindValue(0, bookCode);
    select.bindValue(1, lang->code());
    select.exec();
    select.next();

    return select.value(0).toString();
}


QMap<QString, QString> NWTSource::bookNames(const Language* lang) const
{
    QSqlQuery select(_db);
    select.prepare("SELECT bookCode, name FROM bookNames WHERE langCode=?");
    select.addBindValue(lang->code());
    select.exec();

    QMap<QString, QString> names;
    while (select.next())
        names[select.value(0).toString()] = select.value(1).toString();

    return names;
}



QList<int> NWTSource::verseCounts(const QString& bookCode) const
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
