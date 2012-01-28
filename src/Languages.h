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

    Q_PROPERTY(bool loading READ loading NOTIFY loadingChanged)

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


        void loadFromXML(const QString& xml);
        bool loading() { return _loading; }


    public slots:
        void reload();
        void addLanguage(Language* language);
        void clear();


    signals:
        void loaded();
        void loadingChanged();


    private:
        QList<Language*> _languages;

        bool _loading;

        void setLoading(bool loading);
};

#endif // LANGUAGES
