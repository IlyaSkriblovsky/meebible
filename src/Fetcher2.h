#ifndef FETCHER2_H
#define FETCHER2_H

#include <QObject>
#include <QQueue>
#include <QPair>
#include <QSet>
#include <QString>

#include "Place.h"

class QNetworkAccessManager;

class Translation;
class ChapterRequest;

class Fetcher2: public QObject
{
    Q_OBJECT

    Q_PROPERTY(Translation* translation READ translation WRITE setTranslation)

    public:
        explicit Fetcher2(QObject *parent = 0);


        // Destructor wont be called from QML :(
        ~Fetcher2();

        Translation* translation() const { return _translation; }
        void setTranslation(Translation* translation) { _translation = translation; }

    public slots:
        bool start();
        void stop();

    signals:
        void update(double overallPercent);
        void finished();

    private:
        Translation* _translation;

        QQueue<QPair<QString, int> > _queue;
        QSet<ChapterRequest*> _reqs;

        QNetworkAccessManager* _nam;

        int _totalChaptersAvailable;
        int _totalChaptersInTranslation;


        bool _stopped;

        QQueue<QPair<Place, QString> > _commitQueue;


        void dispatch();
        void commit();


    private slots:
        void onChapterRequestFinished(QString html);
};

#endif
