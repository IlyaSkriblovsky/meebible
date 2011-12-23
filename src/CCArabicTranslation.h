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
        virtual QString name() const { return QString::fromUtf8("<<???>>"); }
        virtual QString sourceUrl() const { return "<<???>>"; }
        virtual QString copyright() const { return QString::fromUtf8("© <<???>>"); }

        virtual QString bookName(const QString &bookCode) const;
        Q_INVOKABLE virtual QStringList bookCodes() const;

        virtual QList<int> verseCounts(const QString& bookCode) const;

        virtual ChapterRequest* requestChapter(QNetworkAccessManager* nam, const QString &bookCode, int chapterNo);


    private:
        const Language* _language;

        QSqlDatabase _db;

        mutable QStringList _bookCodes;
        mutable QMap<QString, QList<int> > _verseCounts;
        mutable QMap<QString, QString> _bookNames;
};

#endif
