#ifndef CHAPTERREQUEST_H
#define CHAPTERREQUEST_H

#include <QtNetwork>


class Translation;


class ChapterRequest: public QObject
{
    Q_OBJECT

public:
    ChapterRequest(
        Translation *translation,
        const QString& bookCode,
        int chapterNo,
        QNetworkReply *nreply
    );

    Translation* translation() const;
    QString bookCode() const;
    int chapterNo() const;


signals:
    void finished(QString html);


protected:
    QNetworkReply *_nreply;


protected slots:
    virtual void onNReplyFinished();


private:
    Translation* _translation;
    QString _bookCode;
    int _chapterNo;
};

#endif // CHAPTERREQUEST_H
