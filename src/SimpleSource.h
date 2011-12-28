#ifndef SIMPLESOURCE_H
#define SIMPLESOURCE_H


#include "Source.h"

class QNetworkAccessManager;

class ChapterRequest;
class Language;
class Translation;
class SimpleTranslation;

class SimpleSource: public Source
{
    public:
        SimpleSource(const QString& langCode);

        virtual QString code() const = 0;
        virtual QString name() const = 0;
        virtual QString sourceUrl() const = 0;
        virtual QString copyright() const = 0;

        virtual void addTranslationsToList(Languages* languages);

        virtual ChapterRequest* requestChapter(SimpleTranslation* translation, QNetworkAccessManager *nam, const QString &bookCode, int chapter) = 0;

    protected:
        Translation* createTranslation(const Language* language);

    private:
        QString _langCode;
};

#endif
