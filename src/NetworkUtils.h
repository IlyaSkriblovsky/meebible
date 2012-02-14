#ifndef NETWORKUTILS_H
#define NETWORKUTILS_H

#include <QString>

class NetworkUtils
{
    public:
        static bool networkAvailable();

        static QString uniqueHardwareId();
};

#endif
