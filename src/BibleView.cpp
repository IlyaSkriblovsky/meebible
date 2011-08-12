#include "BibleView.h"

#include <QDebug>
#include <QFile>

#include <QWebFrame>
#include <QNetworkAccessManager>

#include <QElapsedTimer>

#include "ChapterRequest.h"
#include "Language.h"
#include "Translation.h"
#include "Cache.h"
#include "BibleWebPage.h"
#include "Paths.h"


BibleView::BibleView(QGraphicsItem *parent):
    QGraphicsWebView(parent), _translation(0), _chapterNo(0)
{
    QElapsedTimer timer;
    timer.start();

    qDebug() << "BibleView::BibleView()";

    setPage(new BibleWebPage(this));

    QFile css(Paths::style_css());
    css.open(QIODevice::ReadOnly);
    QByteArray cssContent = css.readAll();

    settings()->setUserStyleSheetUrl(QUrl(
        QString("data:text/css;charset=utf-8;base64,%1").arg(
            QString::fromAscii(cssContent.toBase64())
        )
    ));

    QFile script(Paths::script_js());
    script.open(QIODevice::ReadOnly);
    _js = QString::fromUtf8(script.readAll());


    _nam = new QNetworkAccessManager(this);


    connect(page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()), this, SLOT(onJavaScriptWindowObjectCleared()));
    connect(this, SIGNAL(loadFinished(bool)), this, SLOT(onLoadFinished(bool)));

    qDebug() << "BibleView():" << timer.elapsed();
}


BibleView::~BibleView()
{
    qDebug() << "~~~ BibleView::~BibleView()";
}


Translation* BibleView::translation() const
{
    return _translation;
}

void BibleView::setTranslation(Translation *translation)
{
    if (_translation == translation) return;

    QString origBookName;

    if (_translation)
        origBookName = _translation->bookName(_bookCode);

    _translation = translation;

    loadChapter();

    translationChanged();
}


void BibleView::setAndLoad(const QString& bookCode, int chapterNo)
{
    setBookCode(bookCode);
    setChapterNo(chapterNo);

    loadChapter();
}

void BibleView::loadChapter()
{
    if (_translation == 0 || _bookCode.length() == 0)
    {
        setUrl(QUrl::fromLocalFile(Paths::welcome_html()));
        return;
    }

    if (! Place(_bookCode, _chapterNo).isValid(_translation))
    {
//        clearDisplay(origBookName.isEmpty() ? "" : QString("This translation doesn't contain %1").arg(origBookName));
        clearDisplay("Current translation doesn't contain this book");
        qDebug() << "Absent" << _translation << _bookCode << _chapterNo;
        return;
    }

    QString fromCache = Cache::instance()->loadChapter(_translation, _bookCode, _chapterNo);

    if (! fromCache.isEmpty())
    {
        displayHtml(fromCache);
    }
    else
    {
        ChapterRequest* request = _translation->requestChapter(_nam, _bookCode, _chapterNo);

        if (request)
            connect(request, SIGNAL(finished(QString)), this, SLOT(onChapterRequestFinished(QString)));

        loading();
    }
}


void BibleView::onChapterRequestFinished(QString html)
{
    ChapterRequest* request = dynamic_cast<ChapterRequest*>(sender());

    if (request->error() != QNetworkReply::NoError)
    {
        clearDisplay("<h3>Cannot load chapter</h3> Please check your internet conncetion");
        chapterLoadingError();
    }
    else
    {
        if (request->bookCode() == _bookCode && request->chapterNo() == _chapterNo)
            displayHtml(html);

        Cache::instance()->saveChapter(
            request->translation(),
            request->bookCode(),
            request->chapterNo(),
            html
        );
    }

    request->deleteLater();
}


void BibleView::displayHtml(QString html)
{
    setHtml(html);
    chapterLoaded();
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

    setAndLoad(next.bookCode(), next.chapterNo());
}

void BibleView::loadNextChapter()
{
    if (! validLocation()) return;

    Place prev = Place(_bookCode, _chapterNo).nextChapter(_translation);

    setAndLoad(prev.bookCode(), prev.chapterNo());
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



int BibleView::preferredWidth()
{
    return page()->preferredContentsSize().width();
}

void BibleView::setPreferredWidth(int width)
{
    page()->setPreferredContentsSize(QSize(width, 100));
}
