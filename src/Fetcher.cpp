#include "Fetcher.h"

#include <QNetworkAccessManager>
#include <QDebug>

#include "Translation.h"
#include "ChapterRequest.h"
#include "Cache.h"


Fetcher::Fetcher(QObject *parent):
    QObject(parent), _translation(0)
{
    init();
}


Fetcher::Fetcher(Translation* translation, QObject *parent):
    QObject(parent), _translation(translation)
{
    init();
}


void Fetcher::init()
{
    _nam = new QNetworkAccessManager(this);

    _running = false;
    _currentBook = 0;
}


Translation* Fetcher::translation() const
{
    return _translation;
}

void Fetcher::setTranslation(Translation* translation)
{
    _translation = translation;
}


bool Fetcher::start()
{
    if (_running && !_stopped) return false;

    _stopped = false;
    _running = true;

    QStringList bookCodes = _translation->bookCodes();

    _chaptersInTranslation = 0;
    for (int i = 0; i < bookCodes.size(); ++i)
        _chaptersInTranslation += _translation->chaptersInBook(bookCodes[i]);

    _overallFinishedChapters = Cache::instance()->totalChaptersInCache(_translation);

    qDebug() << "chaptersInTranslation" << _chaptersInTranslation;
    qDebug() << "overallFinishedChapters" << _overallFinishedChapters;

    nextBook(0);

    if (! _running)
    {
        // nextBook decided that we have nothing to do
        qDebug() << "Nothing to download";
        return false;
    }

    return true;
}


void Fetcher::stop()
{
    _stopped = true;
}


void Fetcher::nextBook(int book)
{
    if (_stopped)
    {
        return;
    }

    if (book == -1)
        _currentBook += 1;
    else
        _currentBook = 0;

    if (_currentBook == _translation->bookCodes().size())
    {
        finish();
        return;
    }

    QString bookCode = _translation->bookCodes()[_currentBook];

    _finishedChapters = 0;
    _chaptersInCurrentBook = _translation->chaptersInBook(bookCode);

    for (int chapter = 1; chapter <= _chaptersInCurrentBook; chapter++)
    {
        Place place(bookCode, chapter);

        if (! Cache::instance()->hasChapter(_translation, place))
        {
            ChapterRequest* request = _translation->requestChapter(_nam, place);
            request->setParent(this);
            connect(request, SIGNAL(finished(QString)), this, SLOT(onChapterRequestFinished(QString)));
        }
        else
            _finishedChapters += 1;
    }

    if (_finishedChapters == _chaptersInCurrentBook)
        nextBook();
}


void Fetcher::finish()
{
    _running = false;
    finished();
}


void Fetcher::onChapterRequestFinished(QString html)
{
    _finishedChapters += 1;
    _overallFinishedChapters += 1;

    ChapterRequest* request = dynamic_cast<ChapterRequest*>(sender());
    request->deleteLater();

    double bookPercent = (double)_finishedChapters / _chaptersInCurrentBook;
    double overallPercent = (double)_overallFinishedChapters / _chaptersInTranslation;
    Place place = request->place();
    update(_translation->bookName(place.bookCode()), bookPercent, overallPercent);


    if (request->error() == QNetworkReply::NoError)
        Cache::instance()->saveChapter(_translation, place, html);
    else
        qDebug() << "Error" << request->error();


    if (_finishedChapters == _chaptersInCurrentBook)
    {
        nextBook();
    }
}
