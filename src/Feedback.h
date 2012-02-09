#ifndef FEEDBACK_H
#define FEEDBACK_H

#include <QObject>

class QNetworkAccessManager;
class QNetworkReply;

class Feedback: public QObject
{
    Q_OBJECT

    public:
        Feedback();


    public slots:
        void send(
            const QString& name,
            const QString& email,
            const QString& message
        );

        void abort();


    signals:
        void success();
        void fail(bool aborted);


    private slots:
        void onRequestFinished();


    private:
        QNetworkAccessManager* _nam;
        QNetworkReply* _reply;
};

#endif
