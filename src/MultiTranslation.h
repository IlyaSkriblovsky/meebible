#ifndef MULTITRANSLATION_H
#define MULTITRANSLATION_H

#include "Translation.h"

class Language;
class MultiSource;


class MultiTranslation: public Translation
{
    public:
        MultiTranslation(
            MultiSource* source,
            Language* language,
            const QString& code,
            const QString& name,
            const QString& sourceUrl,
            const QString& copyright
        );

        virtual const Language* language() const { return _language; }

        virtual QString code() const { return _code; }
        virtual QString name() const { return _name; }
        virtual QString sourceUrl() const { return _sourceUrl; }
        virtual QString copyright() const { return _copyright; }

        Q_INVOKABLE virtual QString bookName(const QString& bookCode) const;
        Q_INVOKABLE virtual QStringList bookCodes() const;

        virtual QList<int> verseCounts(const QString& bookCode) const;

        virtual ChapterRequest* requestChapter(QNetworkAccessManager* nam, const QString& bookCode, int chapterNo);


    private:
        MultiSource *_source;
        Language* _language;

        QString _code;
        QString _name;
        QString _sourceUrl;
        QString _copyright;
};

#endif
