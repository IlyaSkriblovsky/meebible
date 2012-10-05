#ifndef CACHE_H
#define CACHE_H

#include <QObject>
#include <QRegExp>
#include <QVariant>
#include <QSet>
#include <QPair>

#include "Place.h"
#include "Indexer.h"

#include "CacheStorage.h"
#include "SearchThread.h"


class QThread;


class Translation;


class Cache: public QObject
{
    Q_OBJECT

    public:
        static Cache* instance();

        Cache();
        virtual ~Cache();

        void saveChapter(const Translation* translation, const Place& place, QString html);
        void saveChapters(const Translation* translation, QList<QPair<Place, QString> > chapters);
        void syncIndex();

        QString loadChapter(const Translation* translation, const Place& place);

        bool hasChapter(const Translation* translation, const Place& place);

        int totalChaptersInCache(const Translation* translation);


        QSet<QPair<QString, int> > availableChapters(const Translation* translation);




        void saveXML(const QString& name, const QString& content);
        bool hasXML(const QString& name);
        QString loadXML(const QString& name);


        QString stripHtml(const QString& html);


        bool oldStorageFound();
        void convertOldCacheDB();


        void closeStorage();
        void closeIndex();


    public slots:
        void clearCache();


        void search(Translation* translation, const QString& query, int maxresults);
        // void clearIndex(Translation* translation);

    signals:
        void searchFinished(QList<QVariant> found);
        void rebuildingIndex();
        void indexRebuilt();


    private:
        static Cache* _instance;


        QRegExp _stripTags;
        QRegExp _stripSpaces;
        QRegExp _stripStyles;

        Indexer _indexer;


        CacheStorage *_storage;
        QString _storageBasename;


        void openStorage(const Translation* translation);
        void openStorage(const QString& transCode, const QString& langCode);
        void fillStorageHeader(ChapterStorageHeader *header, const Place& place);


        // void execWithCheck(const char* sql);

        // void openDB();
        // void closeDB();


    private slots:
        void onSearchThreadFinished();
        void onIndexRebuilt();
};

#endif // CACHE_H
