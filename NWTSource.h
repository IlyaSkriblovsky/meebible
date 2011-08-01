#ifndef NWTSOURCE_H
#define NWTSOURCE_H

#include "Source.h"

#include <QSqlDatabase>
#include <QMap>
#include <QStringList>

class QNetworkReply;


class Languages;
class Language;


class NWTSource : public Source
{
public:
    NWTSource();

    virtual void addTranslationsToList(Languages* languages);


    QString bookName(const Language *lang, const QString& bookCode) const;
    QStringList bookCodes() const;
    bool hasBook(const QString& bookCode) const;

    int chaptersInBook(const QString& bookCode) const;
    int versesInChapter(const QString& bookCode, int chapterNo) const;


    QMap<QString, QString> bookNames(const Language *lang) const;


private:
    QSqlDatabase _db;

    QStringList _bookCodes;
};

#endif // NWTSOURCE_H
