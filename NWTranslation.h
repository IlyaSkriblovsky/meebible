#ifndef NWTRANSLATION_H
#define NWTRANSLATION_H

#include <QNetworkAccessManager>


#include "Translation.h"


class Language;
class NWTSource;


class NWTranslation : public Translation
{
public:
    NWTranslation(const NWTSource *source, const Language *lang, const QString& urlPrefix);
    virtual ~NWTranslation();

    virtual const Language* language() const;

    virtual QString code() const;
    virtual QString name() const;

    virtual QString bookName(const QString& bookCode) const;
    virtual QList<QString> bookCodes() const;
    virtual bool hasBook(const QString& bookCode) const;

    virtual int chaptersInBook(const QString &bookCode) const;
    virtual int versesInChapter(const QString &bookCode, int chapterNo) const;

    QString chapterUrl(const QString& bookCode, int chapterNo) const;

    virtual ChapterRequest* requestChapter(const QString &bookCode, int chapterNo);

private:
    const NWTSource* _source;
    const Language* _lang;
    QString _urlPrefix;
};

#endif // NWTRANSLATION_H
