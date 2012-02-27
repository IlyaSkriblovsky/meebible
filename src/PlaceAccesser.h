#ifndef PLACEACCESSER_H
#define PLACEACCESSER_H

// This class is used from QML to access fields of Place objects

#include <QObject>
#include <QString>

#include "Place.h"

class PlaceAccesser: public QObject
{
    Q_OBJECT

    public slots:
        QString bookCode(const Place& place);
        int chapterNo(const Place& place);
        QList<int> verses(const Place& place);

        QString string(const Place& place, Translation* translation);

        Place placeNoVerses(const QString& bookCode, int chapterNo);
        Place placeOneVerse(const QString& bookCode, int chapterNo, int verse);
};

#endif
