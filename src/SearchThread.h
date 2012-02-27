#ifndef SEARCHTHREAD_H
#define SEARCHTHREAD_H

#include <QThread>
#include <QSqlDatabase>

#include "Place.h"

class Translation;

class SearchThread: public QThread
{
    Q_OBJECT

public:
    SearchThread(const Translation* translation, const QString& needle, QObject* parent = 0);
    ~SearchThread();

signals:
    void matchFound(Place place, QString match, int matchCount);

protected:
    void run();

private:
    QSqlDatabase _db;
    QString _needle;
    QString _langCode;
    QString _transCode;
};

#endif
