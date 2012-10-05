#pragma once

#include <QObject>
#include <QSet>
#include <QList>

#include "IndexTree.h"

class Translation;

class Indexer: public QObject
{
    Q_OBJECT

    Q_PROPERTY(const Translation* translation READ translation WRITE setTranslation)

    public:
        Indexer(QObject* parent = 0);
        ~Indexer();

        const Translation* translation() const { return _translation; }
        void setTranslation(const Translation*);

        void clear();
        void addChapter(int bookNo, int chapterNo, const QString& content);
        void sync();

        unsigned int docCount();

        QMap<QPair<int, int>, QList<MatchEntry> > search(const QString& query, int cutoffDistance);

    private:
        IndexTree* _indexFile;

        const Translation* _translation;


        struct AddChapterClosure {
            IndexTree* index;
            int bookNo;
            int chapterNo;
        };

        static void onDocumentToken(void* data, const QString& token, int startPos, int endPos, int number);


        struct QueryParseClosure {
            QList<QString> tokens;
        };

        static void onQueryToken(void* data, const QString& token, int startPos, int endPos, int number);
};
