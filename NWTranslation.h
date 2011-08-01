#ifndef NWTRANSLATION_H
#define NWTRANSLATION_H

#include <QNetworkAccessManager>
#include <QMap>


#include "Translation.h"


class Language;
class NWTSource;


class NWTranslation : public Translation
{
    Q_OBJECT

public:
    NWTranslation(const NWTSource *source, const Language *lang, const QString& urlPrefix);
    virtual ~NWTranslation();

    virtual const Language* language() const;

    virtual QString code() const;
    virtual QString name() const;

    virtual QString bookName(const QString& bookCode) const;
    Q_INVOKABLE virtual QStringList bookCodes() const;

    virtual QList<int> verseCounts(const QString& bookCode) const;

    QString chapterUrl(const QString& bookCode, int chapterNo) const;

    virtual ChapterRequest* requestChapter(QNetworkAccessManager* nam, const QString &bookCode, int chapterNo);

private:
    const NWTSource* _source;
    const Language* _lang;
    QString _urlPrefix;

    mutable QMap<QString, QString> _bookNames;
};

#endif // NWTRANSLATION_H
