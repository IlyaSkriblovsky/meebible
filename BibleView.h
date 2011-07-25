#ifndef BIBLEVIEW_H
#define BIBLEVIEW_H

#include <QWebView>

class QNetworkAccessManager;

#include "Place.h"

class Translation;


class BibleView: public QWebView
{
    Q_OBJECT

public:
    explicit BibleView(QWidget *parent = 0);

    void setTranslation(Translation *translation);
    Translation* translation() const;

    QString bookCode() const { return _bookCode; }
    int chapterNo() const { return _chapterNo; }
    bool validLocation() const;

    Place selectedPlace();

public slots:
    void loadChapter(const QString& bookCode, int chapterNo);

    void loadNextChapter();
    void loadPrevChapter();

private:
    Translation* _translation;

    QString _bookCode;
    int _chapterNo;

    QString _js;

    QNetworkAccessManager* _nam;

private slots:
    void onChapterRequestFinished(QString html);

    void displayHtml(QString html);
    void clearDisplay(const QString& error = QString());

    void onJavaScriptWindowObjectCleared();
    void onLoadFinished(bool ok);
};

#endif // BIBLEVIEW_H
