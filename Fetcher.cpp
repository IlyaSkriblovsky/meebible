#include "Fetcher.h"

#include <QNetworkAccessManager>
#include <QDebug>

#include "Translation.h"
#include "ChapterRequest.h"
#include "Cache.h"


Fetcher::Fetcher(Translation* translation, QObject *parent):
    QObject(parent), _translation(translation)
{
    _nam = new QNetworkAccessManager(this);

    _running = false;
    _currentBook = 0;

    _bookCodes = _translation->bookCodes();

    _chaptersInTranslation = 0;
    for (int i = 0; i < _bookCodes.size(); ++i)
        _chaptersInTranslation += _translation->chaptersInBook(_bookCodes[i]);
}


bool Fetcher::start()
{
    if (_running) return false;

    _running = true;

    _overallFinishedChapters = Cache::instance()->totalChaptersInCache(_translation);

    qDebug() << "chaptersInTranslation" << _chaptersInTranslation;
    qDebug() << "overallFinishedChapters" << _overallFinishedChapters;

    nextBook(0);

    if (! _running)
    {
        // nextBook decided that we have nothing to do
        return false;
    }

    return true;
}


void Fetcher::nextBook(int chapter)
{
    if (chapter == -1)
        _currentBook += 1;
    else
        _currentBook = 0;

    if (_currentBook == _bookCodes.size())
    {
        finish();
        return;
    }

    QString bookCode = _bookCodes[_currentBook];

    _finishedChapters = 0;
    _chaptersInCurrentBook = _translation->chaptersInBook(bookCode);

    for (int chapter = 1; chapter <= _chaptersInCurrentBook; chapter++)
    {
        if (! Cache::instance()->hasChapter(_translation, bookCode, chapter))
        {
            ChapterRequest* request = _translation->requestChapter(_nam, bookCode, chapter);
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
    qDebug() << "finish";
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
    update(_translation->bookName(request->bookCode()), bookPercent, overallPercent);
    qDebug() << _translation->bookName(request->bookCode()) << request->chapterNo() << bookPercent << overallPercent;


    if (request->error() == QNetworkReply::NoError)
        Cache::instance()->saveChapter(_translation, request->bookCode(), request->chapterNo(), html);


    if (_finishedChapters == _chaptersInCurrentBook)
    {
        nextBook();
    }
}
