#include "StartupTracker.h"

#include <QDebug>

#include <QNetworkAccessManager>
#include <QNetworkRequest>


#include "NetworkUtils.h"
#include "Utils.h"
#include "Paths.h"
#include "Settings.h"
#include "Language.h"
#include "Translation.h"


void StartupTracker::sendStartupInfo()
{
    if (! NetworkUtils::networkAvailable())
        return;

    QUrl params;
    params.addQueryItem("os", Utils::osName());
    params.addQueryItem("device_id", NetworkUtils::uniqueHardwareId());

    Language* lang = Settings::instance()->language();
    if (lang)
        params.addQueryItem("lang", lang->code());

    Translation* trans = Settings::instance()->translation();
    if (trans)
        params.addQueryItem("trans", trans->code());

    QNetworkRequest request(Paths::wsUrl("startup"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    QNetworkAccessManager* nam = new QNetworkAccessManager(this);
    nam->post(request, params.encodedQuery());
}
