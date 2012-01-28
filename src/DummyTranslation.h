#ifndef DUMMYTRANSLATION_H
#define DUMMYTRANSLATION_H

#include "Translation.h"

#include <QDebug>

#include "Language.h"


class QNetworkAccessManager;


class DummyTranslation: public Translation
{
    Q_OBJECT

    public:
        struct BookInfo
        {
            QString code;
            QString name;
            QList<int> chapterSize;
        };


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


        virtual QString bookName(const QString& bookCode) const;
        virtual QStringList bookCodes() const;

        QList<int> verseCounts(const QString& bookCode) const;

        virtual ChapterRequest* requestChapter(QNetworkAccessManager* nam, const QString& bookCode, int chapterNo);


        void loadFromXML(const QString& xml);


    public slots:
        void reload();

    signals:
        void loaded();


    private:
        QString _code;

        Language* _language;

        QString _name;
        QString _sourceUrl;
        QString _copyright;

        QMap<QString, BookInfo> _books;
        QStringList _bookCodes;


        void addBookInfo(const BookInfo& bookInfo);

        friend class TranslationInfoParser;
};

#endif
