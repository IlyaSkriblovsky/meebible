#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class BibleView;
class Translation;


class MainWindow: public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);

    void setTranslation(Translation* translation);

    void showChapter(const QString& bookCode, int chapterNo);

private:
    BibleView* _bibleView;
};

#endif // MAINWINDOW_H
