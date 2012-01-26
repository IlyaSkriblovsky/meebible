#ifndef LANGUAGE_H
#define LANGUAGE_H

#include <QObject>
#include <QString>

class Language: public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString code READ code NOTIFY codeChanged)
    Q_PROPERTY(QString engname READ engname NOTIFY engnameChanged)
    Q_PROPERTY(QString selfname READ selfname NOTIFY selfnameChanged)

public:
    Language(const QString& code, const QString& engname, const QString& selfname);

    QString code() const { return _code; }
    QString engname() const { return _engname; }
    QString selfname() const { return _selfname; }


signals:
    void codeChanged();
    void engnameChanged();
    void selfnameChanged();

private:
    QString _code;
    QString _engname;
    QString _selfname;
};

#endif // LANGUAGE_H
