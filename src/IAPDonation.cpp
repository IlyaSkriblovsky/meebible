#include "IAPDonation.h"

#include <iapclient.h>

#include <QDebug>
#include <QElapsedTimer>


IAPDonation::IAPDonation(QObject* parent): QObject(parent)
{
    QElapsedTimer timer; timer.start();

    _iap = new IAPClient(this);
    connect(_iap, SIGNAL(purchaseCompleted(int, QString, QString)), this, SLOT(onPurchaseCompleted(int, QString, QString)));
}

void IAPDonation::purchase(QString productId)
{
    _iap->purchaseProduct(productId, IAPClient::NoForcedRestoration);
}

void IAPDonation::onPurchaseCompleted(int requestId, QString status, QString ticket)
{
    qDebug() << "onPurchaseCompleted";
    qDebug() << "\t" << "requestId" << requestId;
    qDebug() << "\t" << "status" << status;
    qDebug() << "\t" << "ticket" << ticket;
}
