#include "MultiSource.h"

#include <QCoreApplication>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>

#include "Paths.h"
#include "Languages.h"
#include "Language.h"
#include "MultiTranslation.h"
#include "Parser.h"
#include "ParserFactory.h"


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
    QSqlQuery select("SELECT t.transCode, langCode, parser, name, sourceUrl, copyright, rtl FROM translations AS t LEFT JOIN translationLangs AS tl ON t.transCode = tl.transCode", _db);
    select.exec();
    qDebug() << "exec" << 1;

    while (select.next())
    {
        QString code = select.value(0).toString();
        Language *lang = languages->langByCode(select.value(1).toString());
        QString parser = select.value(2).toString();
        QString name = select.value(3).toString();
        QString sourceUrl = select.value(4).toString();
        QString copyright = select.value(5).toString();
        bool rtl = select.value(6).toBool();

        MultiTranslation *trans = new MultiTranslation(
            this, lang, parser,
            code, name, sourceUrl, copyright, rtl
        );
        lang->addTranslation(trans);
    }
}


QString MultiSource::bookName(const Translation* translation, const QString& bookCode) const
{
    QPair<const Translation*, QString> cachePair(translation, bookCode);

    if (_cache_bookName.contains(cachePair))
        return _cache_bookName.value(cachePair);

    QSqlQuery query("SELECT bookName FROM books WHERE transCode=:transCode AND langCode=:langCode AND bookCode=:bookCode", _db);
    query.bindValue(0, translation->code());
    query.bindValue(1, translation->language()->code());
    query.bindValue(2, bookCode);
    query.exec();
    qDebug() << "exec" << 2;

    if (query.next())
    {
        _cache_bookName.insert(cachePair, query.value(0).toString());

        return query.value(0).toString();
    }
    else
        return "<unknown>";
}

QStringList MultiSource::bookCodes(const Translation* translation) const
{
    if (_cache_bookCodes.contains(translation))
        return _cache_bookCodes.value(translation);

    QSqlQuery query("SELECT bookCode FROM books WHERE transCode=:transCode AND langCode=:langCode ORDER BY bookNo", _db);
    query.bindValue(0, translation->code());
    query.bindValue(1, translation->language()->code());
    query.exec();
    qDebug() << "exec" << 3;

    QStringList result;
    while (query.next())
        result << query.value(0).toString();

    qDebug() << result;

    _cache_bookCodes.insert(translation, result);

    return result;
}

QList<int> MultiSource::verseCounts(const Translation* translation, const QString& bookCode) const
{
    QPair<const Translation*, QString> cachePair(translation, bookCode);

    if (_cache_verseCounts.contains(cachePair))
        return _cache_verseCounts.value(cachePair);

    QSqlQuery query("SELECT verseCount FROM chapterSize WHERE transCode=:transCode AND bookCode=:bookCode ORDER BY chapterNo", _db);
    query.bindValue(0, translation->code());
    query.bindValue(1, bookCode);
    query.exec();
    qDebug() << "exec" << 4;

    QList<int> result;
    while (query.next())
        result << query.value(0).toInt();

    _cache_verseCounts.insert(cachePair, result);

    return result;
}


ChapterRequest* MultiSource::requestChapter(Translation* translation, QNetworkAccessManager *nam, const QString &bookCode, int chapterNo)
{
    MultiTranslation* mt = dynamic_cast<MultiTranslation*>(translation);

    Parser* parser = ParserFactory::getParser(mt->parser());

    if (parser == 0)
    {
        qCritical() << "Unknown parser" << mt->parser();
        QCoreApplication::exit(1);
    }

    return parser->requestChapter(this, translation, nam, bookCode, chapterNo);
}
