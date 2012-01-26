#ifndef DUMMYTRANSLATION_H
#define DUMMYTRANSLATION_H

#include "Translation.h"
#include "Language.h"

class DummyTranslation: public Translation
{
    public:
        DummyTranslation(
            const QString& code,
            Language* language,
            const QString& name,
            const QString& sourceUrl,
            const QString& copyright
        );
        ~DummyTranslation();


        Language* language() const { return _language; }


        virtual QString code() const { return _code; }
        virtual QString name() const { return _name; }
        virtual QString sourceUrl() const { return _sourceUrl; }
        virtual QString copyright() const { return _copyright; }


        virtual QString bookName(const QString& bookCode) const { return "<bookName>"; }
        virtual QStringList bookCodes() const { return QStringList(); }

        QList<int> verseCounts(const QString& bookCode) const { return QList<int>(); }

        virtual ChapterRequest* requestChapter(QNetworkAccessManager* nam, const QString& bookCode, int chapterNo)
        {
            return 0;
        }


    private:
        QString _code;

        Language* _language;

        QString _name;
        QString _sourceUrl;
        QString _copyright;
};

#endif
