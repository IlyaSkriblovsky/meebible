#include "Fetcher2.h"

#include <QDebug>
#include <QNetworkAccessManager>
#include <QElapsedTimer>

#include "Translation.h"
#include "ChapterRequest.h"
#include "Cache.h"


#define MAX_CONCURRENT  20
#define COMMIT_BATCH    50


Fetcher2::Fetcher2(QObject *parent)
    : QObject(parent), _translation(0)
{
    _nam = new QNetworkAccessManager(this);
}


bool Fetcher2::start()
{
    Q_ASSERT(_translation != 0);

    _queue.clear();
    _commitQueue.clear();
    _reqs.clear();

    QSet<QPair<QString, int> > avail = Cache::instance()->availableChapters(_translation);

    _totalChaptersAvailable = avail.size();
    _totalChaptersInTranslation = 0;

    foreach(const QString& bookCode, _translation->bookCodes())
    {
        int chapters = _translation->chaptersInBook(bookCode);
        _totalChaptersInTranslation += chapters;
        for (int i = 1; i <= chapters; i++)
        {
            QPair<QString, int> pair(bookCode, i);
            if (! avail.contains(pair))
                _queue.enqueue(pair);
        }
    }

    if (_queue.size() == 0)
        return false;

    _stopped = false;
    dispatch();

    return true;
}

void Fetcher2::stop()
{
    _stopped = true;

    foreach(ChapterRequest *req, _reqs)
        req->abort();

    commit();
}



void Fetcher2::dispatch()
{
    Q_ASSERT(_translation != 0);

    if (_commitQueue.size() >= COMMIT_BATCH)
        commit();

    if (_stopped)
        return;

    if (_reqs.size() == 0 && _queue.size() == 0)
    {
        commit();
        finished();
        return;
    }

    while (_reqs.size() < MAX_CONCURRENT)
    {
        if (_queue.size() == 0)
            return;

        QPair<QString, int> chapter = _queue.dequeue();
        Place place(chapter.first, chapter.second);

        ChapterRequest* req = _translation->requestChapter(_nam, place);
        req->setParent(this);
        connect(req, SIGNAL(finished(QString)), this, SLOT(onChapterRequestFinished(QString)));

        _reqs.insert(req);
    }
}


void Fetcher2::commit()
{
    Cache* cache = Cache::instance();

    QElapsedTimer timer; timer.start(); int s = _commitQueue.size();
    cache->beginTransaction();
    while (_commitQueue.size() > 0)
    {
        QPair<Place, QString> task = _commitQueue.dequeue();
        cache->saveChapter(_translation, task.first, task.second);
    }
    cache->commitTransaction();
    cache->syncIndex();
    qDebug() << "storing to fts of" << s << "chaps:" << timer.elapsed();
}


void Fetcher2::onChapterRequestFinished(QString html)
{
    ChapterRequest* request = dynamic_cast<ChapterRequest*>(sender());
    _reqs.remove(request);
    request->deleteLater();

    if (request->error() == QNetworkReply::NoError)
    {
        _commitQueue.enqueue(QPair<Place, QString>(request->place(), html));
        _totalChaptersAvailable += 1;

        update((double)_totalChaptersAvailable / _totalChaptersInTranslation);
    }
    else
        if (request->error() != QNetworkReply::OperationCanceledError)
            qWarning() << "Fetcher error:" << request->error();

    dispatch();
}
