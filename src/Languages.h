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
    Q_PROPERTY(bool loaded READ loaded NOTIFY loadedChanged)

    public:
        enum Roles {
            CodeRole = Qt::UserRole + 1,
            EngnameRole,
            SelfnameRole
        };


        static Languages* instance() { return _instance; }


        Languages();
        ~Languages();

        Q_INVOKABLE Language* langByCode(const QString& code);

        virtual int rowCount(const QModelIndex& index = QModelIndex()) const;
        virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

        Q_INVOKABLE Language* langAt(int row) const;


        void loadFromXML(const QString& xml);
        bool loading() { return _loading; }
        bool loaded() { return _languages.size() > 0; }


    public slots:
        void reload(bool useCache);
        void addLanguage(Language* language);


    signals:
        void loadingFinished();
        void loadingError();
        void loadingChanged();
        void loadedChanged();


    private slots:
        void metaXMLReceived();
        void clear();


    private:
        static Languages* _instance;

        QList<Language*> _languages;

        bool _loading;

        void setLoading(bool loading);
};

#endif // LANGUAGES
