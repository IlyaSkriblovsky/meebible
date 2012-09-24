#include "IndexRebuildThread.h"

#include "Translation.h"
#include "Cache.h"


IndexRebuildThread::IndexRebuildThread(
    Indexer* indexer,
    CacheStorage* storage,
    Translation* translation,
    Cache* parent
) : QThread(parent), _indexer(indexer), _storage(storage), _bookCodes(translation->bookCodes())
{
}



void IndexRebuildThread::run()
{
    Cache* cache = dynamic_cast<Cache*>(parent());

    int count = _storage->entryCount();

    ChapterStorageHeader *headers = new ChapterStorageHeader[count];

    _storage->getAllHeaders(headers);

    _indexer->clear();

    for (int i = 0; i < count; i++)
    {
        QByteArray content = _storage->load(headers[i]);

        _indexer->addChapter(
            _bookCodes.indexOf(QString(headers[i].bookCode)),
            headers[i].chapterNo,
            cache->stripHtml(QString::fromUtf8(content))
        );
    }


    delete[] headers;

    finished();
}
