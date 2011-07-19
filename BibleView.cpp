#include "BibleView.h"

#include <QDebug>
#include <QFile>

#include "ChapterRequest.h"
#include "Translation.h"
#include "Cache.h"


BibleView::BibleView(QWidget *parent):
    QWebView(parent), _translation(0)
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


void BibleView::setTranslation(Translation *translation)
{
    _translation = translation;
}

void BibleView::loadChapter(const QString& bookCode, int chapterNo)
{
    QByteArray fromCache = Cache::instance()->loadChapter(_translation, bookCode, chapterNo);

    if (! fromCache.isEmpty())
        displayHtml(fromCache);
    else
    {
        ChapterRequest* request = _translation->requestChapter(bookCode, chapterNo);
        connect(request, SIGNAL(finished(QByteArray)), this, SLOT(onChapterRequestFinished(QByteArray)));
    }
}


void BibleView::onChapterRequestFinished(QByteArray html)
{
    ChapterRequest* request = dynamic_cast<ChapterRequest*>(sender());

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
