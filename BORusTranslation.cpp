#include "BORusTranslation.h"

#include <QDebug>
#include <QCoreApplication>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QNetworkAccessManager>
#include <QVariant>

#include "Language.h"
#include "BOChapterRequest.h"

BORusTranslation::BORusTranslation()
{
    _db = QSqlDatabase::addDatabase("QSQLITE", "bo");
    _db.setDatabaseName("../MeeBible2/bo.sqlite");
    if (! _db.open())
    {
        qDebug() << "Cannot open bo db";
        QCoreApplication::exit(1);
    }

    _nam = new QNetworkAccessManager(this);
}


const Language* BORusTranslation::language() const
{
    return Language::langByCode("u");
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
    QSqlQuery select(_db);
    select.prepare("SELECT name FROM books WHERE bookCode=:bookCode");
    select.addBindValue(bookCode);
    select.exec();
    select.next();

    return select.value(0).toString();
}

QList<QString> BORusTranslation::bookCodes() const
{
    QList<QString> result;

    QSqlQuery select("SELECT booCode FROM books ORDER BY no", _db);
    while (select.next())
        result.append(select.value(0).toString());

    return result;
}

bool BORusTranslation::hasBook(const QString &bookCode) const
{
    QSqlQuery select(_db);
    select.prepare("SELECT count(*) FROM books WHERE bookCode=:bookCode");
    select.addBindValue(bookCode);
    select.exec();
    select.next();

    return select.value(0).toInt() != 0;
}


int BORusTranslation::chaptersInBook(const QString &bookCode) const
{
    QSqlQuery select(_db);
    select.prepare("SELECT count(*) FROM chapterSize WHERE bookCode=:bookCode");
    select.addBindValue(bookCode);
    select.exec();
    select.next();

    return select.value(0).toInt();
}

int BORusTranslation::versesInChapter(const QString &bookCode, int chapterNo) const
{
    QSqlQuery select(_db);
    select.prepare("SELECT verses FROM chapterSize WHERE bookCode=:bookCode AND chapterNo=:chapterNo");
    select.addBindValue(bookCode);
    select.addBindValue(chapterNo);
    select.exec();

    if (! select.next())
        qDebug() << "Cannot load verseCount for bo bookCode=" << bookCode << " chapterNo=" << chapterNo;

    return select.value(0).toInt();
}

QString BORusTranslation::chapterUrl(const QString &bookCode, int chapterNo) const
{
    QSqlQuery select(_db);
    select.prepare("SELECT no FROM books WHERE bookCode=:bookCode");
    select.addBindValue(bookCode);
    select.exec();
    select.next();

    return QString("http://bibleonline.ru/bible/rus/%1/%2/")
            .arg(select.value(0).toInt(), 2, 10, QChar('0'))
            .arg(chapterNo, 2, 10, QChar('0'));
}


ChapterRequest* BORusTranslation::requestChapter(const QString &bookCode, int chapterNo)
{
    return new BOChapterRequest(
        this, bookCode, chapterNo,
        _nam->get(QNetworkRequest(QUrl(
            chapterUrl(bookCode, chapterNo)
        )))
    );
}
