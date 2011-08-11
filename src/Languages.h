#ifndef LANGUAGES_H
#define LANGUAGES_H

#include <QList>
#include <QAbstractListModel>
#include <QHash>
#include <QByteArray>

#include "Language.h"

class Languages: public QAbstractListModel
{
    Q_OBJECT

public:

    enum Roles {
        CodeRole = Qt::UserRole + 1,
        EngnameRole,
        SelfnameRole
    };


    Languages();
    ~Languages();

    Q_INVOKABLE Language* langByCode(const QString& code);

    virtual int rowCount(const QModelIndex& index = QModelIndex()) const;
    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

    Q_INVOKABLE Language* langAt(int row) const;

private:
    QList<Language*> _languages;
};

#endif // LANGUAGES
