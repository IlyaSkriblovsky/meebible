#ifndef CCARABICTRANSLATION_H
#define CCARABICTRANSLATION_H

#include "Translation.h"

#include <QSqlDatabase>

class QNetworkAccessManager;

class Language;

class CCArabicTranslation: public Translation
{
    Q_OBJECT

    public:
        CCArabicTranslation(const Language* language);

        virtual const Language* language() const { return _language; }

        virtual QString code() const { return "ccarabic"; }
        virtual QString name() const { return QString::fromUtf8("Smith & Van Dyke Arabic Version"); }
        virtual QString sourceUrl() const { return "http://www.copticchurch.net/cgibin/bible/"; }
        virtual QString copyright() const { return QString::fromUtf8(""); }

        virtual QString bookName(const QString &bookCode) const;
        Q_INVOKABLE virtual QStringList bookCodes() const;

        virtual QList<int> verseCounts(const QString& bookCode) const;

        virtual ChapterRequest* requestChapter(QNetworkAccessManager* nam, const QString &bookCode, int chapterNo);


        virtual bool rtl() const { return true; }


    private:
        const Language* _language;

        QSqlDatabase _db;

        mutable QStringList _bookCodes;
        mutable QMap<QString, QList<int> > _verseCounts;
        mutable QMap<QString, QString> _bookNames;
        mutable QMap<QString, QString> _bookSearchNames;

        QString bookSearchName(const QString &bookCode) const;
};

#endif
