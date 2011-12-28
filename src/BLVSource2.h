#ifndef BLVSOURCE2_H
#define BLVSOURCE2_H

#include "SimpleSource.h"

class BLVSource2: public SimpleSource
{
    public:
        BLVSource2();
        virtual ~BLVSource2();

        virtual QString code() const { return "blv"; }
        virtual QString name() const { return QString::fromUtf8("1965. gada revidētā Bībeles versija"); }
        virtual QString sourceUrl() const { return "http://bibele.lv/bibele/bibele.php"; }
        virtual QString copyright() const { return QString::fromUtf8("© Latvijas Bībeles Biedrība"); }

        virtual ChapterRequest* requestChapter(SimpleTranslation* translation, QNetworkAccessManager *nam, const QString &bookCode, int chapter);
};

#endif
