#ifndef MULTISOURCE_H
#define MULTISOURCE_H

#include "Source.h"

#include <QString>
#include <QSqlDatabase>

class Languages;
class ChapterRequest;
class Translation;
class QNetworkAccessManager;


class MultiSource: public Source
{
    public:
        MultiSource(const QString& dbname);

        virtual void addTranslationsToList(Languages* languages);

        virtual ChapterRequest* requestChapter(Translation* translation, QNetworkAccessManager *nam, const QString &bookCode, int chapter) = 0;


        virtual QString bookName(const Translation* translation, const QString& bookCode) const;
        virtual QStringList bookCodes(const Translation* translation) const;

        virtual QList<int> verseCounts(const Translation* translation, const QString& bookCode) const;

    private:
        QSqlDatabase _db;
};

#endif
