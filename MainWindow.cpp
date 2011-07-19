#include "MainWindow.h"

#include <QToolBar>

#include "BibleView.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QToolBar* toolbar = new QToolBar(this);

    _bibleView = new BibleView;

    setCentralWidget(_bibleView);
}

void MainWindow::setTranslation(Translation *translation)
{
    _bibleView->setTranslation(translation);
}


void MainWindow::showChapter(const QString &bookCode, int chapterNo)
{
    _bibleView->loadChapter(bookCode, chapterNo);
}
