#ifndef BOSOURCE2_H
#define BOSOURCE2_H

#include "SimpleSource.h"

class BOSource2: public SimpleSource
{
    public:
        BOSource2();
        virtual ~BOSource2();

        virtual QString code() const { return "bo"; }
        virtual QString name() const { return QString::fromUtf8("Синодальный перевод"); }
        virtual QString sourceUrl() const { return "http://bibleonline.ru"; }
        virtual QString copyright() const { return QString::fromUtf8(""); }

        virtual ChapterRequest* requestChapter(SimpleTranslation* translation, QNetworkAccessManager *nam, const QString &bookCode, int chapter);

    private:
        QString chapterUrl(SimpleTranslation* translation, const QString& bookCode, int chapterNo) const;
};

#endif
