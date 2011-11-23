#ifndef BLVTRANSLATION_H
#define BLVTRANSLATION_H

#include "Translation.h"

class QNetworkAccessManager;

class Language;

class BLVTranslation: public Translation
{
    Q_OBJECT

    public:
        BLVTranslation(const Language* language);

        virtual const Language* language() const { return _language; }

        virtual QString code() const { return "blv"; }
        virtual QString name() const { return "?? LATVIAN ??"; }

        virtual QString bookName(const QString &bookCode) const;
        Q_INVOKABLE virtual QStringList bookCodes() const;

        virtual QList<int> verseCounts(const QString& bookCode);

        virtual ChapterRequest* requestChapter(QNetworkAccessManager* nam, const QString &bookCode, int chapterNo);


    private:
        Language* _language;

        QSqlDatabase _db;

        mutable QStringList _bookCodes;
        mutable QMap<QString, QList<int> > _verseCounts;
        mutable QMap<QString, QString> _bookNames;
};

#endif
