#include "SimpleTranslation.h"

#include <QCoreApplication>
#include <QSqlQuery>
#include <QByteArray>

#include <QDebug>

#include "Paths.h"
#include "SimpleSource.h"


//
// This class uses lazy loading of data. Do NOT use _bookCodes, _bookNames
// or _verseCounts directly anywhere (except in corresponding getters).
// Always use public getters which would load data from DB on first call.
//

SimpleTranslation::SimpleTranslation(SimpleSource* source, const Language* language)
    :_source(source), _language(language)
{
    _db = QSqlDatabase::addDatabase("QSQLITE", source->code());
    _db.setDatabaseName(Paths::dbByCode(source->code()));
    if (! _db.open())
    {
        qCritical() << "Cannot open" << source->code() << "DB";
        QCoreApplication::exit(1);
    }
}


QString SimpleTranslation::code() const { return _source->code(); }
QString SimpleTranslation::name() const { return _source->name(); }
QString SimpleTranslation::sourceUrl() const { return _source->sourceUrl(); }
QString SimpleTranslation::copyright() const { return _source->copyright(); }


QString SimpleTranslation::bookName(const QString &bookCode) const
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

QStringList SimpleTranslation::bookCodes() const
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


QList<int> SimpleTranslation::verseCounts(const QString& bookCode) const
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

ChapterRequest* SimpleTranslation::requestChapter(QNetworkAccessManager *nam, const QString &bookCode, int chapter)
{
    return _source->requestChapter(this, nam, bookCode, chapter);
}
