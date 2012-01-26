#ifndef LANGUAGES_H
#define LANGUAGES_H

#include <QAbstractListModel>

class QNetworkAccessManager;
class QNetworkReply;

class Language;

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


    public slots:
        void reload();

        void addLanguage(Language* language);


    signals:
        void loaded();


    private slots:
        void requestFinished(QNetworkReply *reply);


    private:
        QList<Language*> _languages;

        QNetworkAccessManager *_nam;
};

#endif
