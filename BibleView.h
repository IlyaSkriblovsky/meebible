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

public:
    explicit BibleView(QGraphicsItem *parent = 0);
    ~BibleView();

    Q_INVOKABLE void setTranslation(Translation *translation);
    Translation* translation() const;

    QString bookCode() const { return _bookCode; }
    int chapterNo() const { return _chapterNo; }
    bool validLocation() const;

    Place selectedPlace();

    int preferredWidth();
    void setPreferredWidth(int width);

public slots:
    void loadChapter(const QString& bookCode, int chapterNo);

    void loadNextChapter();
    void loadPrevChapter();


signals:
    void chapterLoaded();
    void loading();

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
