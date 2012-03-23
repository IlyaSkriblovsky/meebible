#ifndef IAPDONATION_H
#define IAPDONATION_H

#include <QObject>

class IAPClient;

class IAPDonation: public QObject
{
    Q_OBJECT

    public:
        IAPDonation(QObject* parent = 0);

    public slots:
        void purchase(QString productId);

    private:
        IAPClient* _iap;

    private slots:
        void onPurchaseCompleted(int requestId, QString status, QString ticket);
};

#endif
