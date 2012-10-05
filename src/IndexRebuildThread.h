#pragma once

#include <QThread>
#include <QStringList>

#include "CacheStorage.h"


class Translation;
class Indexer;
class Cache;


class IndexRebuildThread: public QThread
{
    Q_OBJECT

    public:
        IndexRebuildThread(
            Indexer* indexer,
            CacheStorage* storage,
            Translation* translation,
            Cache* parent
        );

    signals:
        void finished();

    protected:
        virtual void run();

    private:
        Indexer* _indexer;
        CacheStorage* _storage;
        QStringList _bookCodes;
};
