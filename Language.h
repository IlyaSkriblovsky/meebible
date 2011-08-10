#ifndef LANGUAGE_H
#define LANGUAGE_H

#include <QAbstractListModel>

class Translation;


class Language: public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(QString code READ code NOTIFY codeChanged)
    Q_PROPERTY(QString engname READ engname NOTIFY engnameChanged)
    Q_PROPERTY(QString selfname READ selfname NOTIFY selfnameChanged)

public:
    enum Roles {
        TranslationRole = Qt::UserRole + 1,
        TranslationNameRole
    };

    Language(const QString& code, const QString& engname, const QString& selfname);

    QString code() const { return _code; }
    QString engname() const { return _engname; }
    QString selfname() const { return _selfname; }

    void addTranslation(Translation* translation);


    virtual int rowCount(const QModelIndex& index = QModelIndex()) const;
    virtual QVariant data(const QModelIndex& index, int role) const;

    Q_INVOKABLE Translation* translationAt(int row) const;

    Translation* translationByCode(const QString& code) const;


signals:
    void codeChanged();
    void engnameChanged();
    void selfnameChanged();

private:
    QString _code;
    QString _engname;
    QString _selfname;

    QList<Translation*> _translations;
};

#endif // LANGUAGE_H
