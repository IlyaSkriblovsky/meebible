#include "NWTSource.h"

#include <QtCore>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QNetworkReply>


#include "Languages.h"
#include "NWTranslation.h"


NWTSource::NWTSource()
{
    _db = QSqlDatabase::addDatabase("QSQLITE", "nwt");
    _db.setDatabaseName("../MeeBible2/nwt.sqlite");
    if (! _db.open())
    {
        qDebug() << "Cannot open nwt db";
        QCoreApplication::exit(1);
    }



    QSqlQuery select("SELECT bookCode FROM books ORDER BY no", _db);
    while (select.next())
        _bookCodes.append(select.value(0).toString());
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
    return _bookCodes;
}

bool NWTSource::hasBook(const QString& bookCode) const
{
    QSqlQuery select(_db);
    select.prepare("SELECT count(*) FROM books WHERE bookCode=?");
    select.addBindValue(bookCode);
    select.exec();
    select.next();

    return select.value(0).toInt() != 0;
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


int NWTSource::chaptersInBook(const QString &bookCode) const
{
    QSqlQuery select(_db);
    select.prepare("SELECT count(*) FROM chapterSize WHERE bookCode=?");
    select.bindValue(0, bookCode);
    select.exec();
    select.next();

    return select.value(0).toInt();
}


int NWTSource::versesInChapter(const QString &bookCode, int chapterNo) const
{
    QSqlQuery select(_db);
    select.prepare("SELECT verses FROM chapterSize WHERE bookCode=? AND chapterNo=?");
    select.bindValue(0, bookCode);
    select.bindValue(1, chapterNo);
    select.exec();
    select.next();

    return select.value(0).toInt();
}
