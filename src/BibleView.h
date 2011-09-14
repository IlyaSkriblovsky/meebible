#ifndef BIBLEVIEW_H
#define BIBLEVIEW_H

#include <QGraphicsWebView>

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

public:
    explicit BibleView(QGraphicsItem *parent = 0);
    ~BibleView();

    Q_INVOKABLE void setTranslation(Translation *translation);
    Translation* translation() const;

    QString bookCode() const { return _bookCode; }
    int chapterNo() const { return _chapterNo; }
    bool validLocation() const;

    void setBookCode(const QString& bookCode) { _bookCode = bookCode; bookCodeChanged(); }
    void setChapterNo(int chapterNo) { _chapterNo = chapterNo; chapterNoChanged(); }

    Place selectedPlace();

    int preferredWidth();
    void setPreferredWidth(int width);

public slots:
    void loadChapter();
    void setAndLoad(const QString& bookCode, int chapterNo, int verseNo);

    void loadNextChapter();
    void loadPrevChapter();


signals:
    void chapterLoaded();
    void chapterLoadingError();
    void loading();

    void translationChanged();

    void bookCodeChanged();
    void chapterNoChanged();

    void needToScroll(int y);

private:
    Translation* _translation;

    QString _bookCode;
    int _chapterNo;
    int _verseNo;

    QString _js;

    QNetworkAccessManager* _nam;

private slots:
    void onChapterRequestFinished(QString html);

    void displayHtml(QString html, int verseNo = 0);
    void clearDisplay(const QString& error = QString());

    void onJavaScriptWindowObjectCleared();
    void onLoadFinished(bool ok);
};

#endif // BIBLEVIEW_H
