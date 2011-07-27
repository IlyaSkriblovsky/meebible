#include "BibleView.h"

#include <QDebug>
#include <QFile>

#include <QWebFrame>
#include <QNetworkAccessManager>

#include "ChapterRequest.h"
#include "Translation.h"
#include "Cache.h"
#include "BibleWebPage.h"


BibleView::BibleView(QGraphicsItem *parent):
    QGraphicsWebView(parent), _translation(0), _chapterNo(0)
{
    setPage(new BibleWebPage(this));

    QFile css(":/style.css");
    css.open(QIODevice::ReadOnly);
    QByteArray cssContent = css.readAll();

    settings()->setUserStyleSheetUrl(QUrl(
        QString("data:text/css;charset=utf-8;base64,%1").arg(
            QString::fromAscii(cssContent.toBase64())
        )
    ));

    QFile script(":/script.js");
    script.open(QIODevice::ReadOnly);
    _js = QString::fromUtf8(script.readAll());


    _nam = new QNetworkAccessManager(this);


    connect(page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()), this, SLOT(onJavaScriptWindowObjectCleared()));
    connect(this, SIGNAL(loadFinished(bool)), this, SLOT(onLoadFinished(bool)));
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
        clearDisplay(origBookName.isEmpty() ? "" : QString("This translation doesn't contain %1").arg(origBookName));
}


void BibleView::loadChapter(const QString& bookCode, int chapterNo)
{
    if (! _translation)
    {
        qDebug() << "BibleView::loadChapter while translation == null";
        return;
    }

    _bookCode = bookCode;
    _chapterNo = chapterNo;

    QString fromCache = Cache::instance()->loadChapter(_translation, bookCode, chapterNo);

    if (! fromCache.isEmpty())
        displayHtml(fromCache);
    else
    {
        ChapterRequest* request = _translation->requestChapter(_nam, bookCode, chapterNo);

        if (request)
            connect(request, SIGNAL(finished(QString)), this, SLOT(onChapterRequestFinished(QString)));
    }
}


void BibleView::onChapterRequestFinished(QString html)
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


void BibleView::displayHtml(QString html)
{
    setHtml(html);
}

void BibleView::clearDisplay(const QString& error)
{
    setHtml("<h2>" + error + "</h2>");
}


bool BibleView::validLocation() const
{
    if (_translation == 0) return false;
    return Place(_bookCode, _chapterNo).isValid(_translation);
}


void BibleView::loadPrevChapter()
{
    if (! validLocation()) return;

    Place next = Place(_bookCode, _chapterNo).prevChapter(_translation);

    loadChapter(next.bookCode(), next.chapterNo());
}

void BibleView::loadNextChapter()
{
    if (! validLocation()) return;

    Place prev = Place(_bookCode, _chapterNo).nextChapter(_translation);

    loadChapter(prev.bookCode(), prev.chapterNo());
}



void BibleView::onJavaScriptWindowObjectCleared()
{
    //
}

void BibleView::onLoadFinished(bool ok)
{
    Q_UNUSED(ok)

    page()->mainFrame()->evaluateJavaScript(_js);
}


Place BibleView::selectedPlace()
{
    if (! validLocation()) return Place();

    QStringList jsList = page()->mainFrame()->evaluateJavaScript("selectedVersesList()").toString().split(",", QString::SkipEmptyParts);

    QSet<int> verses;
    for (int i = 0; i < jsList.size(); i++)
        verses += jsList[i].toInt();

    return Place(_bookCode, _chapterNo, verses);
}
