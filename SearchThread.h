#ifndef SEARCHTHREAD_H
#define SEARCHTHREAD_H

#include <QThread>
#include <QSqlDatabase>

class Translation;

class SearchThread: public QThread
{
    Q_OBJECT

public:
    SearchThread(const Translation* translation, const QString& needle, QObject* parent = 0);
    ~SearchThread();

signals:
    void matchFound(QString bookCode, int chapterNo, QString match);

protected:
    void run();

private:
    QSqlDatabase _db;
    QString _needle;
    QString _langCode;
    QString _transCode;
};

#endif
