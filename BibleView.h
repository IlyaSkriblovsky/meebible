#ifndef BIBLEVIEW_H
#define BIBLEVIEW_H

#include <QWebView>

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

public slots:
    void loadChapter(const QString& bookCode, int chapterNo);

    void loadNextChapter();
    void loadPrevChapter();

private:
    Translation* _translation;

    QString _bookCode;
    int _chapterNo;

private slots:
    void onChapterRequestFinished(QString html);

    void displayHtml(QString html);
    void clearDisplay(const QString& error = QString());
};

#endif // BIBLEVIEW_H
