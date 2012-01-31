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
