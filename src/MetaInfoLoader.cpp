#include "MetaInfoLoader.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>

#include "Paths.h"
#include "Languages.h"
#include "DummyTranslation.h"
#include "MetaInfoParser.h"
#include "TranslationInfoParser.h"
#include "Cache.h"



MetaInfoLoader* MetaInfoLoader::_instance = 0;

MetaInfoLoader* MetaInfoLoader::instance()
{
    if (_instance)
        return _instance;

    return new MetaInfoLoader;
}

MetaInfoLoader::MetaInfoLoader()
{
    if (_instance)
        qWarning() << "Duplicate MetaInfoLoader instance";
    _instance = this;

    _nam = new QNetworkAccessManager(this);
}



bool MetaInfoLoader::loadLangsAndTransInfo(Languages *languages)
{
    if (Cache::instance()->hasXML("meta"))
    {
        qDebug() << "LangsAndTrans CACHED";
        languages->loadFromXML(Cache::instance()->loadXML("meta"));
        return false;
    }

    qDebug() << "LangsAndTrans LOADING";

    QNetworkReply* reply = _nam->get(QNetworkRequest(Paths::wsUrl("meta")));
    reply->setProperty("languages", QVariant::fromValue((QObject*)languages));
    connect(reply, SIGNAL(finished()), this, SLOT(metaXMLReceived()));

    return true;
}


void MetaInfoLoader::metaXMLReceived()
{
    QNetworkReply* reply = dynamic_cast<QNetworkReply*>(sender());
    reply->deleteLater();

    QString content = QString::fromUtf8(reply->readAll());

    Languages* languages = dynamic_cast<Languages*>(reply->property("languages").value<QObject*>());
    languages->loadFromXML(content);

    Cache::instance()->saveXML("meta", content);
}


bool MetaInfoLoader::loadTranslationInfo(DummyTranslation* translation)
{
    QString xmlName = QString("%1_%2").arg(translation->code(), translation->language()->code());
    if (Cache::instance()->hasXML(xmlName))
    {
        qDebug() << "Translation" << xmlName << "CACHED";
        translation->loadFromXML(Cache::instance()->loadXML(xmlName));
        return false;
    }

    qDebug() << "Translation" << xmlName << "LOADING";

    QNetworkReply* reply = _nam->get(QNetworkRequest(
        QString("%1?trans=%2&lang=%3")
            .arg(Paths::wsUrl("translation").toString())
            .arg(translation->code())
            .arg(translation->language()->code())
    ));
    reply->setProperty("translation", QVariant::fromValue((QObject*)translation));
    connect(reply, SIGNAL(finished()), this, SLOT(translationXMLReceived()));

    return true;
}

void MetaInfoLoader::translationXMLReceived()
{
    QNetworkReply* reply = dynamic_cast<QNetworkReply*>(sender());
    reply->deleteLater();

    QString content = QString::fromUtf8(reply->readAll());

    DummyTranslation* translation = dynamic_cast<DummyTranslation*>(reply->property("translation").value<QObject*>());
    translation->loadFromXML(content);

    QString xmlName = QString("%1_%2").arg(translation->code(), translation->language()->code());
    Cache::instance()->saveXML(xmlName, content);
}
