#ifndef BLVTRANSLATION_H
#define BLVTRANSLATION_H

#include "Translation.h"

#include <QSqlDatabase>

class QNetworkAccessManager;

class Language;

class BLVTranslation: public Translation
{
    Q_OBJECT

    public:
        BLVTranslation(const Language* language);

        virtual const Language* language() const { return _language; }

        virtual QString code() const { return "blv"; }
        virtual QString name() const { return QString::fromUtf8("1965. gada revidētā Bībeles versija"); }
        virtual QString sourceUrl() const { return "http://bibele.lv/bibele/bibele.php"; }
        virtual QString copyright() const { return "© Latvijas Bībeles Biedrība"; }

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
