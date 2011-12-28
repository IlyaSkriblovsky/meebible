#ifndef SIMPLETRANSLATION_H
#define SIMPLETRANSLATION_H

#include "Translation.h"

#include <QSqlDatabase>

class QNetworkAccessManager;

class Language;
class SimpleSource;

class SimpleTranslation: public Translation
{
    Q_OBJECT

    public:
        SimpleTranslation(SimpleSource *source, const Language* language);

        virtual const Language* language() const { return _language; }

        virtual QString code() const;
        virtual QString name() const;
        virtual QString sourceUrl() const;
        virtual QString copyright() const;

        virtual QString bookName(const QString &bookCode) const;
        Q_INVOKABLE virtual QStringList bookCodes() const;

        virtual QList<int> verseCounts(const QString& bookCode) const;

        virtual ChapterRequest* requestChapter(QNetworkAccessManager *nam, const QString &bookCode, int chapter);


    protected:
        QSqlDatabase _db;


    private:
        SimpleSource* _source;
        const Language* _language;

        mutable QStringList _bookCodes;
        mutable QMap<QString, QList<int> > _verseCounts;
        mutable QMap<QString, QString> _bookNames;
};

#endif
