#ifndef NWTSOURCE_H
#define NWTSOURCE_H

#include "Source.h"

#include <QSqlDatabase>
#include <QNetworkAccessManager>

class QNetworkReply;


class TranslationsList;
class Language;


class NWTSource : public Source
{
public:
    NWTSource();

    virtual void addTranslationsToList(TranslationsList *list);


    QString bookName(const Language *lang, const QString& bookCode) const;
    QList<QString> bookCodes() const;
    bool hasBook(const QString& bookCode) const;

    int chaptersInBook(const QString& bookCode) const;
    int versesInChapter(const QString& bookCode, int chapterNo) const;


private:
    QSqlDatabase _db;
};

#endif // NWTSOURCE_H
