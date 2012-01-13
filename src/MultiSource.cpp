#include "MultiSource.h"

#include <QCoreApplication>
#include <QSqlQuery>
#include <QVariant>
#include <QDebug>

#include "Paths.h"
#include "Languages.h"
#include "Language.h"
#include "MultiTranslation.h"


MultiSource::MultiSource(const QString& dbname)
{
    _db = QSqlDatabase::addDatabase("QSQLITE", dbname);
    _db.setDatabaseName(Paths::dbByCode(dbname));
    if (! _db.open())
    {
        qCritical() << "Cannot open" << dbname << "DB";
        QCoreApplication::exit(1);
    }
}


void MultiSource::addTranslationsToList(Languages* languages)
{
    QSqlQuery select("SELECT transCode, name, sourceUrl, copyright FROM translations", _db);
    select.exec();

    QSqlQuery langs("SELECT langCode FROM translationLangs WHERE transCode = :transCode", _db);

    while (select.next())
    {
        QString code = select.value(0).toString();
        QString name = select.value(1).toString();
        QString sourceUrl = select.value(2).toString();
        QString copyright = select.value(3).toString();

        langs.bindValue(":transCode", code);
        langs.exec();

        while (langs.next())
        {
            Language *lang = languages->langByCode(langs.value(0).toString());
            MultiTranslation *trans = new MultiTranslation(
                this, lang,
                code, name, sourceUrl, copyright
            );
            lang->addTranslation(trans);
        }
    }
}


QString MultiSource::bookName(const Translation* translation, const QString& bookCode) const
{
    QSqlQuery query("SELECT bookName FROM books WHERE transCode=:transCode AND bookCode=:bookCode AND langCode=:langCode", _db);
    query.bindValue(":transCode", translation->code());
    query.bindValue(":langCode", translation->language()->code());
    query.bindValue(":bookCode", bookCode);
    query.exec();

    if (query.next())
        return query.value(0).toString();
    else
        return "<unknown>";
}

QStringList MultiSource::bookCodes(const Translation* translation) const
{
    QSqlQuery query("SELECT bookCode FROM books WHERE transCode=:transCode ORDER BY bookNo", _db);
    query.bindValue(":transCode", translation->code());
    query.exec();

    QStringList result;
    while (query.next())
        result << query.value(0).toString();

    qDebug() << result;

    return result;
}

QList<int> MultiSource::verseCounts(const Translation* translation, const QString& bookCode) const
{
    QSqlQuery query("SELECT verseCount FROM chapterSize WHERE transCode=:transCode AND bookCode=:bookCode ORDER BY chapterNo", _db);
    query.bindValue(":transCode", translation->code());
    query.bindValue(":bookCode", bookCode);
    query.exec();

    QList<int> result;
    while (query.next())
        result << query.value(0).toInt();

    return result;
}
