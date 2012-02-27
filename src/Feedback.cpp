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
    QUrl params;
    params.addQueryItem("name", name);
    params.addQueryItem("email", email);
    params.addQueryItem("message", message);

    QNetworkRequest request(Paths::wsUrl("feedback"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    _reply = _nam->post(request, params.encodedQuery());
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
