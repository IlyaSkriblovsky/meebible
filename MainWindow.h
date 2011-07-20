#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QAction;

class BibleView;
class Translation;
class TranslationsList;


class MainWindow: public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(TranslationsList* transList, QWidget *parent = 0);

    void setTranslation(Translation* translation);

    void showChapter(const QString& bookCode, int chapterNo);

private:
    BibleView* _bibleView;

    QAction* _actSelectTranslation;
    QAction* _actPrevChapter;
    QAction* _actNextChapter;

    QAction* _actDbg;

    TranslationsList* _translationsList;

private slots:
    void onActSelectTranslation();

    void onActDbg();
};

#endif // MAINWINDOW_H
