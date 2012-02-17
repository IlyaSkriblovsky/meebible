#ifndef METAINFOLOADER_H
#define METAINFOLOADER_H

#include <QObject>

class QNetworkAccessManager;

class MetaInfoLoader: public QObject
{
    Q_OBJECT

    public:
        static MetaInfoLoader* instance();

        MetaInfoLoader();

        QNetworkAccessManager* nam() { return _nam; }

    private:
        static MetaInfoLoader* _instance;


        QNetworkAccessManager *_nam;
};

#endif
