#ifndef CHAPTERREQUEST_H
#define CHAPTERREQUEST_H

#include <QNetworkReply>

#include "Place.h"


class Translation;


class ChapterRequest: public QObject
{
    Q_OBJECT

public:
    ChapterRequest(
        Translation *translation,
        const Place& place,
        QNetworkReply *nreply
    );

    Translation* translation() const;
    Place place() const;

    void abort();
    QNetworkReply::NetworkError error() const;


signals:
    void finished(QString html);


protected:
    QNetworkReply *_nreply;


protected slots:
    virtual void onNReplyFinished();


private:
    Translation* _translation;
    Place _place;
};

#endif // CHAPTERREQUEST_H
