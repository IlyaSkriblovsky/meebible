#ifndef METAINFOLOADER_H
#define METAINFOLOADER_H

#include <QObject>

class QNetworkAccessManager;

class Languages;
class DummyTranslation;

class MetaInfoLoader: public QObject
{
    Q_OBJECT

    public:
        static MetaInfoLoader* instance();

        MetaInfoLoader();


        bool loadLangsAndTransInfo(Languages *languages);
        bool loadTranslationInfo(DummyTranslation* translation);


    signals:
        void langsAndTransLoaded();


    private slots:
        void metaXMLReceived();
        void translationXMLReceived();

    private:
        static MetaInfoLoader* _instance;


        QNetworkAccessManager *_nam;
};

#endif
