#ifndef BORUSTRANSLATION_H
#define BORUSTRANSLATION_H

#include "Translation.h"

#include <QSqlDatabase>

class QNetworkAccessManager;


class BORusTranslation : public Translation
{
public:
    BORusTranslation();

    virtual const Language* language() const;

    virtual QString code() const;
    virtual QString name() const;

    virtual QString bookName(const QString &bookCode) const;
    virtual QList<QString> bookCodes() const;
    virtual bool hasBook(const QString &bookCode) const;

    virtual int chaptersInBook(const QString &bookCode) const;
    virtual int versesInChapter(const QString &bookCode, int chapterNo) const;

    QString chapterUrl(const QString& bookCode, int chapterNo) const;

    virtual ChapterRequest* requestChapter(QNetworkAccessManager* nam, const QString &bookCode, int chapterNo);


private:
    QSqlDatabase _db;
};

#endif // BORUSTRANSLATION_H
