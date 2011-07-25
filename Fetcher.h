#ifndef FETCHER_H
#define FETCHER_H

#include <QObject>

class QNetworkAccessManager;

class Translation;


class Fetcher : public QObject
{
    Q_OBJECT

public:
    explicit Fetcher(Translation* translation, QObject *parent = 0);

    bool start();

signals:
    void update(const QString& text, double bookPercent, double overallPercent);
    void finished();

private:
    Translation* _translation;

    QNetworkAccessManager* _nam;

    bool _running;
    int _currentBook;

    int _overallFinishedChapters;
    int _chaptersInTranslation;

    int _chaptersInCurrentBook;
    int _finishedChapters;

    QList<QString> _bookCodes;

    void nextBook(int chapter = -1);
    void finish();

private slots:
    void onChapterRequestFinished(QString html);
};

#endif // FETCHER_H
