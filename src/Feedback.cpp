#include "Feedback.h"

#include <QDebug>

#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

#include "Paths.h"


Feedback::Feedback()
{
    _nam = new QNetworkAccessManager(this);
    _reply = 0;
}


void Feedback::send(const QString& name, const QString& email, const QString& message)
{
    QString body = QString("name=%1&email=%2&message=%3")
        .arg(QString::fromAscii(QUrl::toPercentEncoding(name)))
        .arg(QString::fromAscii(QUrl::toPercentEncoding(email)))
        .arg(QString::fromAscii(QUrl::toPercentEncoding(message)))
    ;

    qDebug() << body;

    _reply = _nam->post(
        QNetworkRequest(Paths::wsUrl("feedback")),
        body.toUtf8()
    );
    connect(_reply, SIGNAL(finished()), this, SLOT(onRequestFinished()));
}


void Feedback::onRequestFinished()
{
    QNetworkReply* reply = dynamic_cast<QNetworkReply*>(sender());
    reply->deleteLater();

    if (reply->error() == QNetworkReply::NoError && reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 200)
        success();
    else
        fail(reply->error() == QNetworkReply::OperationCanceledError);

    if (reply == _reply)
        _reply = 0;
}

void Feedback::abort()
{
    if (_reply)
    {
        _reply->abort();
        delete _reply;
        _reply = 0;
    }
}
