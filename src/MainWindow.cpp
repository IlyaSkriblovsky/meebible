#include "MainWindow.h"

#include <QDebug>
#include <QToolBar>
#include <QAction>

#include "BibleView.h"
#include "Translation.h"
#include "TranslationDialog.h"
#include "FetcherDialog.h"


MainWindow::MainWindow(TranslationsList* transList, QWidget *parent)
    : QMainWindow(parent), _translationsList(transList)
{
    _bibleView = new BibleView;
    setCentralWidget(_bibleView);

    _actSelectTranslation = new QAction("Select translation", this);
    connect(_actSelectTranslation, SIGNAL(triggered()), this, SLOT(onActSelectTranslation()));

    _actPrevChapter = new QAction("Prev", this);
    connect(_actPrevChapter, SIGNAL(triggered()), _bibleView, SLOT(loadPrevChapter()));

    _actNextChapter = new QAction("Next", this);
    connect(_actNextChapter, SIGNAL(triggered()), _bibleView, SLOT(loadNextChapter()));

    _actDbg = new QAction("Dbg", this);
    connect(_actDbg, SIGNAL(triggered()), this, SLOT(onActDbg()));

    QToolBar* toolbar = new QToolBar(this);
    addToolBar(toolbar);
    toolbar->addAction(_actSelectTranslation);
    toolbar->addAction(_actPrevChapter);
    toolbar->addAction(_actNextChapter);
    toolbar->addAction(_actDbg);
}

void MainWindow::setTranslation(Translation *translation)
{
    _bibleView->setTranslation(translation);
}


void MainWindow::showChapter(const QString &bookCode, int chapterNo)
{
    _bibleView->loadChapter(bookCode, chapterNo);
}

void MainWindow::onActSelectTranslation()
{
    TranslationDialog dialog(_translationsList, _bibleView->translation());

    if (dialog.exec() == QDialog::Accepted)
        setTranslation(dialog.selectedTranslation());
}


void MainWindow::onActDbg()
{
    FetcherDialog dialog(_bibleView->translation(), this);

    if (dialog.exec() == QDialog::Accepted)
        qDebug() << "Accepted";
    else
        qDebug() << "Rejected";
}
