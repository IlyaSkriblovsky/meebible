#ifndef CCARABICSOURCE2_H
#define CCARABICSOURCE2_H

#include "SimpleSource.h"

class CCArabicSource2: public SimpleSource
{
    public:
        CCArabicSource2();
        virtual ~CCArabicSource2();

        virtual QString code() const { return "ccarabic"; }
        virtual QString name() const { return QString::fromUtf8("Smith & Van Dyke Arabic Version"); }
        virtual QString sourceUrl() const { return "http://www.copticchurch.net/cgibin/bible/"; }
        virtual QString copyright() const { return QString::fromUtf8(""); }

        virtual ChapterRequest* requestChapter(SimpleTranslation* translation, QNetworkAccessManager *nam, const QString &bookCode, int chapter);

    protected:
        Translation* createTranslation(const Language* language);
};

#endif
