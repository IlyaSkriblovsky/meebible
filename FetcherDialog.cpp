#include "FetcherDialog.h"

#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QProgressBar>

#include "Fetcher.h"

FetcherDialog::FetcherDialog(Translation* translation, QWidget *parent) :
    QDialog(parent)
{
    setWindowTitle("Downloading translation...");

    setMinimumWidth(300);

    _fetcher = new Fetcher(translation, this);

    connect(_fetcher, SIGNAL(update(QString, double, double)), this, SLOT(onFetcherUpdate(QString, double, double)));
    connect(_fetcher, SIGNAL(finished()), this, SLOT(onFetcherFinished()));


    QDialogButtonBox* box = new QDialogButtonBox(QDialogButtonBox::Abort, Qt::Horizontal, this);

    connect(box, SIGNAL(rejected()), this, SLOT(onRejected()));


    _text = new QLabel;
    _bookProgress = new QProgressBar;
    _overallProgress = new QProgressBar;

    QGridLayout* layout = new QGridLayout;
    layout->addWidget(_text, 0, 0, 1, 2);
    layout->addWidget(new QLabel("Book:"), 1, 0);
    layout->addWidget(_bookProgress, 1, 1);
    layout->addWidget(new QLabel("Bible:"), 2, 0);
    layout->addWidget(_overallProgress, 2, 1);

    QVBoxLayout* dialogLayout = new QVBoxLayout(this);
    dialogLayout->addLayout(layout);
    dialogLayout->addWidget(box);
}

int FetcherDialog::exec()
{
    if (_fetcher->start())
        return QDialog::exec();
    else
        return QDialog::Accepted;
}


void FetcherDialog::onFetcherUpdate(const QString &text, double bookPercent, double overallPercent)
{
    _text->setText(text);
    _bookProgress->setValue(bookPercent * 100);
    _overallProgress->setValue(overallPercent * 100);
}

void FetcherDialog::onFetcherFinished()
{
    accept();
}


void FetcherDialog::onRejected()
{
    reject();
}
