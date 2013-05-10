#ifdef SYMBIAN

#include "MessageHelper.h"

#include <QDebug>


MessageHelper::MessageHelper()
{
}

MessageHelper::~MessageHelper()
{
}


void MessageHelper::composeSMS(const QString& body)
{
    QMessage msg;
    msg.setType(QMessage::Sms);
    msg.setBody(body);
    _service.compose(msg);
}

void MessageHelper::composeEmail(const QString& subject, const QString& body)
{
    QMessage msg;
    msg.setType(QMessage::Email);
    msg.setSubject(subject);
    msg.setBody(body);
    _service.compose(msg);
}

#endif
