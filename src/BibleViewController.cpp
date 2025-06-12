#include "BibleViewController.h"

#include <QApplication>
#include <QClipboard>
#include <QDebug>
#include <QFile>

#include <QNetworkAccessManager>

#include <QDesktopServices>

#ifdef MEEGO_SHARE
    #include <MDataUri>
    #include <maemo-meegotouch-interfaces/shareuiinterface.h>
#endif

#include "ChapterRequest.h"
#include "Language.h"
#include "Translation.h"
#include "Cache.h"
// #include "BibleWebPage.h"
#include "Paths.h"
#include "Bookmarks.h"
#include "SearchQueryParser.h"
#include "Highlighter.h"
#include "PlaceAccesser.h"




BibleViewController::BibleViewController(QObject *parent):
    QObject(parent), _webView(nullptr), _translation(0), _fontSize(30), _fontName("Nokia"),
    _loadingChapter(false)
{
    // BibleWebPage* page = new BibleWebPage(this);
    // page->setLinkDelegationPolicy(QWebPage::DelegateExternalLinks);
    // connect(page, SIGNAL(linkClicked(const QUrl&)), this, SLOT(onLinkClicked(const QUrl&)));
    // setPage(page);


    QFile css(Paths::style_css());
    css.open(QIODevice::ReadOnly);
    _css = css.readAll();

    // settings()->setUserStyleSheetUrl(QUrl(
    //     QString("data:text/css;charset=utf-8;base64,%1").arg(
    //         QString::fromAscii(cssContent.toBase64())
    //     )
    // ));

    QFile webChannelJsFile(":///qtwebchannel/qwebchannel.js");
    webChannelJsFile.open(QIODevice::ReadOnly);
    QString webChannelJs = QString::fromUtf8(webChannelJsFile.readAll());

    QFile script(Paths::script_js());
    script.open(QIODevice::ReadOnly);
    _js = webChannelJs + QString::fromUtf8(script.readAll());


    _nam = new QNetworkAccessManager(this);


    // connect(page->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()), this, SLOT(onJavaScriptWindowObjectCleared()));
    // connect(this, SIGNAL(loadFinished(bool)), this, SLOT(onLoadFinished(bool)));


    _searchMode = false;
    _searchNeedle = "";
    _matchCount = 0;
    _matchIndex = 0;
}


BibleViewController::~BibleViewController()
{
}

QObject* BibleViewController::webView() const {
    return _webView;
}
void BibleViewController::setWebView(QObject* webView) {
    _webView = webView;
}

QString BibleViewController::webViewUrl() const {
    // Prepend our custom CSS
    QString fullContent = "<style>" + _css + "</style>" + _html;

    // Encode HTML to UTF-8
    QByteArray utf8Encoded = fullContent.toUtf8();

    // Base64 encode the UTF-8 data
    QByteArray base64Encoded = utf8Encoded.toBase64();

    // Prepend the data URL header
    QString dataUrl = "data:text/html;base64," + QString::fromLatin1(base64Encoded);

    return dataUrl;
}

QString BibleViewController::scriptJS() const {
    return _js;
}


void BibleViewController::setLoadingChapter(bool loading)
{
    if (_loadingChapter != loading)
    {
        _loadingChapter = loading;
        emit loadingChapterChanged();
    }
}


Translation* BibleViewController::translation() const
{
    return _translation;
}

void BibleViewController::setTranslation(Translation *translation)
{
    if (_translation == translation) return;

    _translation = translation;
    translationChanged();

    if (_translation)
    {
        if (_translation->loaded())
        {
            loadChapter();

            titleChanged();
        }
        else
        {
            connect(_translation, SIGNAL(loadingFinished()), this, SLOT(onTranslationLoadingFinished()));

            _translation->reload(true);
        }
    }
}


void BibleViewController::onTranslationLoadingFinished()
{
    disconnect(sender(), SIGNAL(loadingFinished()), this, SLOT(onTranslationLoadingFinished()));

    loadChapter();
    titleChanged();
}



Place BibleViewController::place() const
{
    return _place;
}

void BibleViewController::setPlace(const Place& place)
{
    if (_place == place) return;

    Place old = _place;

    _place = place;
    placeChanged();
    selectedVersesChanged();

    if (! old.sameChapter(place))
    {
        loadChapter();
        titleChanged();
    }
    else
        showSelectedVerses(_place.verses());
}

void BibleViewController::reload()
{
    loadChapter();
}


QList<int> BibleViewController::selectedVerses() const
{
    return _place.verses().values();
}


// void BibleViewController::setAndLoad(const QString& bookCode, int chapterNo, int verseNo)
// {
//     setBookCode(bookCode);
//     setChapterNo(chapterNo);
// 
//     _versesToSelectAfterLoad = QSet<int>();
//     _versesToSelectAfterLoad << verseNo;
// 
//     loadChapter();
// }


// void BibleViewController::loadPlace(const Place& place)
// {
//     setBookCode(place.bookCode());
//     setChapterNo(place.chapterNo());
// 
//     _versesToSelectAfterLoad = place.verses();
// 
//     loadChapter();
// }

void BibleViewController::loadChapter()
{
    if (_translation == 0)
    {
        return;
    }

    if (! _place.isValid(_translation))
    {
//        clearDisplay(origBookName.isEmpty() ? "" : QString("This translation doesn't contain %1").arg(origBookName));
        clearDisplay(tr("Current translation doesn't contain this book"));
        qDebug() << "Absent" << _place.toStringCode();
        return;
    }

    QString fromCache = Cache::instance()->loadChapter(_translation, _place);

    if (! fromCache.isEmpty())
    {
        displayHtml(fromCache);
        showSelectedVerses(_place.verses());
        emit chapterLoaded();
        setLoadingChapter(false);
    }
    else
    {
        ChapterRequest* request = _translation->requestChapter(_nam, _place);

        if (request)
        {
            connect(request, SIGNAL(finished(QString)), this, SLOT(onChapterRequestFinished(QString)));

            setLoadingChapter(true);
        }
    }
}


void BibleViewController::onChapterRequestFinished(QString html)
{
    ChapterRequest* request = dynamic_cast<ChapterRequest*>(sender());

    if (request->error() != QNetworkReply::NoError)
    {
        clearDisplay(tr("<h3>Cannot load chapter</h3> Please check your internet connection"));
        emit chapterLoadingError();
        setLoadingChapter(false);
    }
    else
    {
        if (request->place().sameChapter(_place))
        {
            displayHtml(html);
            showSelectedVerses(_place.verses());
            emit chapterLoaded();
            setLoadingChapter(false);
        }

        Cache::instance()->saveChapter(
            request->translation(),
            request->place(),
            html
        );
        Cache::instance()->syncIndex();
    }

    request->deleteLater();
}


void BibleViewController::displayHtml(QString html)
{
    // qDebug() << "displayHtml" << html;
    _html = html;
    // setHtml(_html);
    emit webViewUrlChanged();

    stopSearchMode();
}

void BibleViewController::showSelectedVerses(QSet<int> verses)
{
    // QList<int> list = verses.values();
    // // qSort(list);
    // std::sort(list.begin(), list.end());

    // QStringList strList;
    // foreach (int v, list)
    //     strList << QString::number(v);


    // if (_webView) {
    //     QMetaObject::invokeMethod(
    //         _webView, "runJavaScript",
    //         Q_ARG(QString, QString("selectVerses([%1])").arg(strList.join(",")))
    //     );
    // }


    // page()->mainFrame()->evaluateJavaScript(
    //     QString("selectVerses([%1])")
    //         .arg(strList.join(","))
    // );
    // if (verses.size() > 0)
    //     needToScroll(page()->mainFrame()->evaluateJavaScript(QString("verseOffset(%1)").arg(list.at(0))).toInt());
    // else
    //     needToScroll(0);
}

void BibleViewController::clearDisplay(const QString& error)
{
    // setHtml("<h2>" + error + "</h2>");
    displayHtml("<h2>" + error + "</h2>");
}


void BibleViewController::loadPrevChapter()
{
    // if (! validLocation()) return;

    // Place next = Place(_bookCode, _chapterNo).prevChapter(_translation);

    // setAndLoad(next.bookCode(), next.chapterNo(), 1);

    if (_place.isValid(_translation))
        setPlace(_place.prevChapter(_translation));
}

void BibleViewController::loadNextChapter()
{
    // if (! validLocation()) return;

    // Place prev = Place(_bookCode, _chapterNo).nextChapter(_translation);

    // setAndLoad(prev.bookCode(), prev.chapterNo(), 1);

    if (_place.isValid(_translation))
        setPlace(_place.nextChapter(_translation));
}



void BibleViewController::onJavaScriptWindowObjectCleared()
{
    // page()->mainFrame()->addToJavaScriptWindowObject("bibleView", this);
}

void BibleViewController::onLoadFinished(bool ok)
{
    Q_UNUSED(ok)

    // page()->mainFrame()->evaluateJavaScript(_js);
    applyFontSize();
    applyFontName();
    applyLineSpacing();
    applyInverted();
}


// Place BibleViewController::selectedPlace()
// {
//     if (! validLocation()) return Place();
// 
//     QStringList jsList = page()->mainFrame()->evaluateJavaScript("selectedVersesList()").toString().split(",", QString::SkipEmptyParts);
// 
//     QSet<int> verses;
//     for (int i = 0; i < jsList.size(); i++)
//         verses += jsList[i].toInt();
// 
//     return Place(_bookCode, _chapterNo, verses);
// }



/////////////////////////////////////////

void BibleViewController::startSearchMode(const QString& needle)
{
    _searchNeedle = needle;
    searchNeedleChanged();

    QList<SearchQueryParser::QueryToken> queryTokens = SearchQueryParser::parseQuery(needle);
    // setHtml(Highlighter::highlight(_html, queryTokens, "<span class=\"match\">", "</span>", 10, &_matchCount));
    displayHtml(Highlighter::highlight(_html, queryTokens, "<span class=\"match\">", "</span>", 10, &_matchCount));

    matchCountChanged();


    if (_searchMode == false)
    {
        _searchMode = true;
        searchModeChanged();
    }

    setMatchIndex(0);
}


void BibleViewController::stopSearchMode()
{
    if (_searchMode == true)
    {
        _searchMode = false;
        searchModeChanged();

        // setHtml(_html) would be better, but it causes scroll jump
        // page()->mainFrame()->evaluateJavaScript(QString("hideAllHighlights()"));
    }
}

void BibleViewController::setMatchIndex(int index)
{
    // ensureVisible(page()->mainFrame()->evaluateJavaScript(QString("highlightMatch(%1)").arg(
    //     index// - _matchCount
    // )).toInt());

    _matchIndex = index;
    matchIndexChanged();
}

/////////////////////////////////////////



void BibleViewController::onLinkClicked(const QUrl& url)
{
    if (url.scheme() == "mailto" || url.scheme() == "http")
        QDesktopServices::openUrl(url);
}




QString BibleViewController::title() const
{
    return _place.toStringChapterOnly(_translation);
}



void BibleViewController::setFontSize(int value)
{
    _fontSize = value;
    applyFontSize();
    fontSizeChanged();
}

void BibleViewController::applyFontSize()
{
    // page()->mainFrame()->evaluateJavaScript(QString("setFontSize(%1)").arg(_fontSize));
}

void BibleViewController::setFontName(const QString& value)
{
    _fontName = value;
    applyFontName();
    fontNameChanged();
}

void BibleViewController::applyFontName()
{
    // page()->mainFrame()->evaluateJavaScript(QString("setFontName('%1')").arg(_fontName));
}

void BibleViewController::setLineSpacing(float value)
{
    _lineSpacing = value;
    applyLineSpacing();
    lineSpacingChanged();
}

void BibleViewController::applyLineSpacing()
{
    // page()->mainFrame()->evaluateJavaScript(QString("setLineSpacing(%1)").arg(_lineSpacing));
}



void BibleViewController::setInverted(bool inverted)
{
    _inverted = inverted;
    applyInverted();
}

void BibleViewController::applyInverted()
{
    // page()->mainFrame()->evaluateJavaScript(QString("setInverted(%1)").arg(_inverted));
}




// QString BibleViewController::selectedText(bool withVerseNumbers)
// {
//     // QString text = page()->mainFrame()->evaluateJavaScript(
//     //     withVerseNumbers ? "selectedText(true)" : "selectedText(false)"
//     // ).toString().trimmed();
//     // text.replace(QString::fromUtf8("\xcc\x81"), "");
//     // text.replace("  ", " ");

//     // return text;
//     return "";
// }


bool BibleViewController::copySelectedVerses(bool withVerseNumbers)
{
    QString text = withVerseNumbers ? _selectedTextWithNumbers : _selectedTextWithoutNumbers; // selectedText(withVerseNumbers);

    if (text == "")
        return false;

    if (withVerseNumbers)
        text = _place.toString(translation()) + ": " + text;

    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(text);
    return true;
}

#ifdef MEEGO_SHARE
bool BibleViewController::shareSelectedVerses(bool withVerseNumbers)
{
    QString text = selectedText(withVerseNumbers);

    if (text == "")
        return false;


    PlaceAccesser placeAccesser;

    text += "\n" + placeAccesser.siteUrl(_place, translation());


    MDataUri duri;
    duri.setMimeType("text/x-uri");
    duri.setTextData(text, "utf-8");
    duri.setAttribute("title", _place.toString(translation()));

    if (! duri.isValid())
    {
        qDebug() << "DURI is NOT valid";
        return false;
    }

    QStringList items;
    items << duri.toString();

    ShareUiInterface shareIf("com.nokia.ShareUi");

    if (! shareIf.isValid())
    {
        qDebug() << "ShareIf is NOT valid";
        return false;
    }


    shareIf.share(items);

    return true;
}
#endif


void BibleViewController::clearSelection()
{
    // page()->mainFrame()->evaluateJavaScript("clearSelection()");
    _place.setVerses(QSet<int>());
    emit placeChanged();
    emit selectedVersesChanged();
}


void BibleViewController::verseSelectionChanged(QList<int> verses, QString textWithoutNumbers, QString textWithNumbers)
{
    _selectedTextWithoutNumbers = textWithoutNumbers;
    _selectedTextWithNumbers = textWithNumbers;

    std::sort(verses.begin(), verses.end());

    _place.setVerses(QSet(verses.begin(), verses.end()));

    emit placeChanged();
    emit selectedVersesChanged();
}



bool BibleViewController::bookmarkSelectedVerses()
{
    QString text = _selectedTextWithoutNumbers; // selectedText(false);
    text.replace("\n", " ");

    return Bookmarks::instance()->addBookmark(_place, text);
}
