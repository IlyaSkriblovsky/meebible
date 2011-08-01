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

    QList<int> verseCounts(const QString& bookCode) const;


    QMap<QString, QString> bookNames(const Language *lang) const;


private:
    QSqlDatabase _db;

    mutable QStringList _bookCodes;

    mutable QMap<QString, QList<int> > _verseCounts;
};

#endif // NWTSOURCE_H
