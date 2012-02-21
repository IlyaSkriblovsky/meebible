#ifndef BIBLEVIEW_H
#define BIBLEVIEW_H

#include <QGraphicsWebView>
#include <QSet>

class QNetworkAccessManager;

#include "Place.h"

class Translation;


class BibleView: public QGraphicsWebView
{
    Q_OBJECT

    Q_PROPERTY(int preferredWidth READ preferredWidth WRITE setPreferredWidth)

    Q_PROPERTY(Translation* translation READ translation WRITE setTranslation NOTIFY translationChanged)

    Q_PROPERTY(QString bookCode READ bookCode WRITE setBookCode NOTIFY bookCodeChanged)
    Q_PROPERTY(int chapterNo READ chapterNo WRITE setChapterNo NOTIFY chapterNoChanged)

    Q_PROPERTY(bool searchMode READ searchMode NOTIFY searchModeChanged)
    Q_PROPERTY(QString searchNeedle READ searchNeedle NOTIFY searchNeedleChanged)
    Q_PROPERTY(int matchCount READ matchCount NOTIFY matchCountChanged)
    Q_PROPERTY(int matchIndex READ matchIndex WRITE setMatchIndex NOTIFY matchIndexChanged)

    Q_PROPERTY(QString title READ title NOTIFY titleChanged)

    Q_PROPERTY(int fontSize READ fontSize WRITE setFontSize NOTIFY fontSizeChanged)
    Q_PROPERTY(float lineSpacing READ lineSpacing WRITE setLineSpacing NOTIFY lineSpacingChanged)
    Q_PROPERTY(QString fontName READ fontName WRITE setFontName NOTIFY fontNameChanged)

    Q_PROPERTY(bool inverted READ inverted WRITE setInverted)

    Q_PROPERTY(bool loadingChapter READ loadingChapter NOTIFY loadingChapterChanged)

    Q_PROPERTY(QList<int> selectedVerses READ selectedVerses NOTIFY selectedVersesChanged)

public:
    explicit BibleView(QGraphicsItem *parent = 0);
    ~BibleView();

    Q_INVOKABLE void setTranslation(Translation *translation);
    Translation* translation() const;

    QString bookCode() const { return _bookCode; }
    int chapterNo() const { return _chapterNo; }
    bool validLocation() const;

    void setBookCode(const QString& bookCode) { _bookCode = bookCode; bookCodeChanged(); titleChanged(); }
    void setChapterNo(int chapterNo) { _chapterNo = chapterNo; chapterNoChanged(); titleChanged(); }

    // Place selectedPlace();

    int preferredWidth();
    void setPreferredWidth(int width);


    bool searchMode() const { return _searchMode; }

    QString searchNeedle() const { return _searchNeedle; }

    int matchCount() const { return _matchCount; }

    int matchIndex() const { return _matchIndex; }
    void setMatchIndex(int index);

    QString title() const;


    int fontSize() const { return _fontSize; }
    void setFontSize(int value);
    QString fontName() const { return _fontName; }
    void setFontName(const QString& value);
    float lineSpacing() const { return _lineSpacing; }
    void setLineSpacing(float value);

    bool inverted() const { return _inverted; }
    void setInverted(bool inverted);

    bool loadingChapter() const { return _loadingChapter; }


    QList<int> selectedVerses() const { return _selectedVerses; }


public slots:
    void loadChapter();
    void setAndLoad(const QString& bookCode, int chapterNo, int verseNo);
    void loadPlace(const Place& place);

    void loadNextChapter();
    void loadPrevChapter();

    #ifndef NOSEARCH
        void startSearchMode(const QString& needle);
        void stopSearchMode();
    #endif

    bool copySelectedVerses();
    bool shareSelectedVerses();
    void bookmarkSelectedVerses();
    void clearSelection();


    // Called from JS
    void verseSelectionChanged(QList<int> list);


signals:
    void chapterLoaded();
    void chapterLoadingError();
    void loadingChapterChanged();

    void translationChanged();

    void bookCodeChanged();
    void chapterNoChanged();

    void needToScroll(int y);
    void ensureVisible(int y);

    void searchModeChanged();
    void searchNeedleChanged();
    void matchCountChanged();
    void matchIndexChanged();

    void titleChanged();

    void fontSizeChanged();
    void fontNameChanged();
    void lineSpacingChanged();

    void selectedVersesChanged();


private:
    Translation* _translation;

    QString _bookCode;
    int _chapterNo;

    QSet<int> _versesToSelectAfterLoad;

    QString _js;
    QString _html;

    QNetworkAccessManager* _nam;


    int _fontSize;
    QString _fontName;
    float _lineSpacing;

    bool _inverted;


    // Search matching

    bool _searchMode;
    QString _searchNeedle;
    int _matchCount;
    int _matchIndex;


    bool _loadingChapter;


    QList<int> _selectedVerses;


    void setLoadingChapter(bool loading);


    void showSelectedVerses(QSet<int> verses);

    void applyFontSize();
    void applyFontName();
    void applyLineSpacing();
    void applyInverted();


    void setSelectedVerses(QList<int> verses);


private slots:
    void onChapterRequestFinished(QString html);

    void displayHtml(QString html);
    void clearDisplay(const QString& error = QString());

    void onJavaScriptWindowObjectCleared();
    void onLoadFinished(bool ok);

    void onLinkClicked(const QUrl& url);

    void onTranslationLoadingFinished();


    QString selectedText();
};

#endif // BIBLEVIEW_H
