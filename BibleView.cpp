#include "BibleView.h"

#include <QDebug>
#include <QFile>

#include "ChapterRequest.h"
#include "Translation.h"
#include "Cache.h"
#include "Place.h"


BibleView::BibleView(QWidget *parent):
    QWebView(parent), _translation(0), _chapterNo(0)
{
    QFile css(":/style.css");
    css.open(QIODevice::ReadOnly);
    QByteArray cssContent = css.readAll();

    settings()->setUserStyleSheetUrl(QUrl(
        QString("data:text/css;charset=utf-8;base64,%1").arg(
            QString::fromAscii(cssContent.toBase64())
        )
    ));
}


Translation* BibleView::translation() const
{
    return _translation;
}

void BibleView::setTranslation(Translation *translation)
{
    QString origBookName;

    if (_translation)
        origBookName = _translation->bookName(_bookCode);

    _translation = translation;

    if (Place(_bookCode, _chapterNo).isValid(_translation))
        loadChapter(_bookCode, _chapterNo);
    else
        clearDisplay(origPlaceName.isEmpty() ? "" : QString("This translation doesn't contain %1").arg(origBookName));
}


void BibleView::loadChapter(const QString& bookCode, int chapterNo)
{
    if (! _translation) return;

    _bookCode = bookCode;
    _chapterNo = chapterNo;

    QByteArray fromCache = Cache::instance()->loadChapter(_translation, bookCode, chapterNo);

    if (! fromCache.isEmpty())
        displayHtml(fromCache);
    else
    {
        ChapterRequest* request = _translation->requestChapter(bookCode, chapterNo);

        if (request)
            connect(request, SIGNAL(finished(QByteArray)), this, SLOT(onChapterRequestFinished(QByteArray)));
    }
}


void BibleView::onChapterRequestFinished(QByteArray html)
{
    ChapterRequest* request = dynamic_cast<ChapterRequest*>(sender());

    if (request->bookCode() == _bookCode && request->chapterNo() == _chapterNo)
        displayHtml(html);

    Cache::instance()->saveChapter(
        request->translation(),
        request->bookCode(),
        request->chapterNo(),
        html
    );

    request->deleteLater();
}


void BibleView::displayHtml(QByteArray html)
{
    setContent(html);
}

void BibleView::clearDisplay(const QString& error)
{
    setHtml("<h2>" + error + "</h2>");
}


void BibleView::loadPrevChapter()
{
    if (_translation == 0) return;

    Place cur = Place(_bookCode, _chapterNo);
    if (! cur.isValid(_translation))
        return;

    Place next = cur.prevChapter(_translation);

    loadChapter(next.bookCode(), next.chapterNo());
}

void BibleView::loadNextChapter()
{
    if (_translation == 0) return;

    Place cur = Place(_bookCode, _chapterNo);
    if (! cur.isValid(_translation))
        return;

    Place prev = cur.nextChapter(_translation);

    loadChapter(prev.bookCode(), prev.chapterNo());
}
