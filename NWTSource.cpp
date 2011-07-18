#include "NWTSource.h"

#include <QtCore>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QNetworkReply>


#include "TranslationsList.h"
#include "Language.h"
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

    _nam = new QNetworkAccessManager(this);
}


void NWTSource::addTranslationsToList(TranslationsList *list)
{
    QSqlQuery select("SELECT langCode, urlPrefix FROM langs", _db);

    while (select.next())
    {
        const Language *lang = Language::langByCode(select.value(0).toString());
        list->addTranslation(
            lang,
            new NWTranslation(
                this,
                lang,
                select.value(1).toString()
            )
        );
    }
}


QList<QString> NWTSource::bookCodes() const
{
    QList<QString> result;

    QSqlQuery select("SELECT bookCode FROM books ORDER BY no", _db);
    while (select.next())
        result.append(select.value(0).toString());

    return result;
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


QNetworkReply* NWTSource::requestChapter(const QString& url) const
{
    return _nam->get(QNetworkRequest(QUrl(url)));
}
