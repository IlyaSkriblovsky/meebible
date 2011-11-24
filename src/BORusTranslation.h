#ifndef BORUSTRANSLATION_H
#define BORUSTRANSLATION_H

#include "Translation.h"

#include <QSqlDatabase>
#include <QMap>

class QNetworkAccessManager;


class Language;


class BORusTranslation : public Translation
{
    Q_OBJECT

public:
    BORusTranslation(const Language* language);

    virtual const Language* language() const;

    virtual QString code() const;
    virtual QString name() const;
    virtual QString sourceUrl() const;
    virtual QString copyright() const;

    virtual QString bookName(const QString &bookCode) const;
    Q_INVOKABLE virtual QStringList bookCodes() const;

    virtual QList<int> verseCounts(const QString& bookCode) const;

    QString chapterUrl(const QString& bookCode, int chapterNo) const;

    virtual ChapterRequest* requestChapter(QNetworkAccessManager* nam, const QString &bookCode, int chapterNo);


private:
    QSqlDatabase _db;

    const Language* _language;

    mutable QStringList _bookCodes;
    mutable QMap<QString, QList<int> > _verseCounts;

    mutable QMap<QString, QString> _bookNames;
};

#endif // BORUSTRANSLATION_H
