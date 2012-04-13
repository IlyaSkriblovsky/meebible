#include "IAPDonation.h"

#include <iapclient.h>

#include <QThread>
#include <QDebug>
#include <QElapsedTimer>


/*
class IAPCreationThread: public QThread
{
    public:
        IAPCreationThread(IAPDonation* parent):
            QThread(parent), _iapDonation(parent)
        {
        }

        void run()
        {
            qDebug() << "run() 1";
            QElapsedTimer timer; timer.start();
            qDebug() << "run() 2";
            IAPClient *iap = new IAPClient;
            qDebug() << "run() 3";
            qDebug() << "ELAPSED:" << timer.elapsed();
            qDebug() << "run() 4";

            _iapDonation->iapCreated(iap);
            qDebug() << "run() 5";
        }

    private:
        IAPDonation* _iapDonation;
};
*/


IAPDonation::IAPDonation(QObject* parent):
    QObject(parent), _iap(0)
{
}

IAPClient* IAPDonation::iap()
{
    if (_iap) return _iap;

    _iap = new IAPClient(this);
    connect(_iap, SIGNAL(purchaseCompleted(int, QString, QString)), this, SLOT(onPurchaseCompleted(int, QString, QString)));
    connect(_iap, SIGNAL(purchaseFlowFinished(int)), this, SLOT(onPurchaseFlowFinished(int)));

    return _iap;
}

void IAPDonation::purchase(QString productId)
{
    iap()->purchaseProduct(productId, IAPClient::NoForcedRestoration);
}

void IAPDonation::onPurchaseCompleted(int requestId, QString status, QString ticket)
{
    purchaseCompleted(status == "OK", status == "RestorableProduct");
}

void IAPDonation::onPurchaseFlowFinished(int requestId)
{
    flowFinished();
}
