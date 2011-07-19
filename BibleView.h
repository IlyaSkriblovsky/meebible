#ifndef BIBLEVIEW_H
#define BIBLEVIEW_H

#include <QWebView>

class Translation;


class BibleView: public QWebView
{
    Q_OBJECT

public:
    explicit BibleView(QWidget *parent = 0);

public slots:
    void setTranslation(Translation *translation);

    void loadChapter(const QString& bookCode, int chapterNo);


private:
    Translation* _translation;

private slots:
    void onChapterRequestFinished(QByteArray html);

    void displayHtml(QByteArray html);
};

#endif // BIBLEVIEW_H
