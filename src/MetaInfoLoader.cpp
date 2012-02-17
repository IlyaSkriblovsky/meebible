#include "MetaInfoLoader.h"

#include <QDebug>

#include <QNetworkAccessManager>



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
