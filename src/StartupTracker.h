#ifndef STARTUPTRACKER_H
#define STARTUPTRACKER_H

#include <QObject>

class StartupTracker: public QObject
{
    Q_OBJECT

    public:
        void sendStartupInfo();
};

#endif
