#include "MainWindow.h"

#include <QToolBar>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QToolBar* toolbar = new QToolBar(this);
}
