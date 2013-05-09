#ifndef MESSAGEHELPER_H
#define MESSAGEHELPER_H

#include <QObject>
#include <QString>

#include <qmessage.h>
#include <qmessageservice.h>

QTM_USE_NAMESPACE


class MessageHelper: public QObject
{
    Q_OBJECT

    public:
        MessageHelper();
        ~MessageHelper();

    public slots:
        void composeSMS(const QString& body);
        void composeEmail(const QString& subject, const QString& body);


    private:
        QMessageService _service;
};

#endif
