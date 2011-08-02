#ifndef FETCHER_H
#define FETCHER_H

#include <QObject>

class QNetworkAccessManager;

#include "Translation.h"


class Fetcher : public QObject
{
    Q_OBJECT

    Q_PROPERTY(Translation* translation READ translation WRITE setTranslation)

public:
    explicit Fetcher(QObject *parent = 0);
    explicit Fetcher(Translation* translation, QObject *parent = 0);

    Translation* translation() const;
    void setTranslation(Translation* translation);

public slots:
    bool start();
    void stop();

signals:
    void update(const QString& text, double bookPercent, double overallPercent);
    void finished();

private:
    Translation* _translation;

    QNetworkAccessManager* _nam;

    bool _running;
    bool _stopped;
    int _currentBook;

    int _overallFinishedChapters;
    int _chaptersInTranslation;

    int _chaptersInCurrentBook;
    int _finishedChapters;

    void init();

    void nextBook(int book = -1);
    void finish();

private slots:
    void onChapterRequestFinished(QString html);
};

#endif // FETCHER_H
