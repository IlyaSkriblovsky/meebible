#ifndef KJBOSOURCE2_H
#define KJBOSOURCE2_H

#include "SimpleSource.h"

class KJBOSource: public SimpleSource
{
    public:
        KJBOSource();
        virtual ~KJBOSource();

        virtual QString code() const { return "kjbo"; }
        virtual QString name() const { return "King James Version (1769)"; }
        virtual QString sourceUrl() const { return "http://www.kingjamesbibleonline.org/"; }
        virtual QString copyright() const { return ""; }

        virtual ChapterRequest* requestChapter(SimpleTranslation* translation, QNetworkAccessManager *nam, const QString &bookCode, int chapter);

    private:
        QString chapterUrl(SimpleTranslation* translation, const QString& bookCode, int chapterNo) const;
};

#endif
