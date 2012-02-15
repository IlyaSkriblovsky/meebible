#include "NetworkUtils.h"

#include <QStringList>

#include <QNetworkConfigurationManager>
#include <QNetworkConfiguration>
#include <QNetworkInterface>



bool NetworkUtils::networkAvailable()
{
    QNetworkConfigurationManager manager;
    return manager.allConfigurations(QNetworkConfiguration::Active).size() > 0;
}


QString NetworkUtils::uniqueHardwareId()
{

    #ifdef Q_OS_LINUX
        // MeeGo
        return QNetworkInterface::interfaceFromName("wlan0").hardwareAddress();
    #else
        QList<QNetworkInterface> ifaces = QNetworkInterface::allInterfaces();
        QStringList ids;
        for (int i = ifaces.size() - 1; i >= 0; i--)
        {
            if (ifaces.at(i).hardwareAddress() != "00:00:00:00:00:00")
                ids.append(ifaces.at(i).hardwareAddress());
        }

        return ids.join("_");
    #endif
}
