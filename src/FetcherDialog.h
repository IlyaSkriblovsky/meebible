#ifndef FETCHERDIALOG_H
#define FETCHERDIALOG_H

#include <QDialog>

class QLabel;
class QProgressBar;

class Translation;
class Fetcher;

class FetcherDialog : public QDialog
{
    Q_OBJECT
public:
    explicit FetcherDialog(Translation* translation, QWidget *parent = 0);

    virtual int exec();

signals:

private slots:
    void onFetcherUpdate(const QString& text, double bookPercent, double overallPercent);
    void onFetcherFinished();

    void onRejected();

private:
    Fetcher* _fetcher;

    QLabel* _text;
    QProgressBar* _bookProgress;
    QProgressBar* _overallProgress;
};

#endif // FETCHERDIALOG_H
